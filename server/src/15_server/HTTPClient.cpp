// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// This class manages socket timeouts by applying the concept of a deadline.
// Some asynchronous operations are given deadlines by which they must complete.
// Deadlines are enforced by an "actor" that persists for the lifetime of the
// client object:
//
//  +----------------+
//  |                |
//  | check_deadline |<---+
//  |                |    |
//  +----------------+    | async_wait()
//              |         |
//              +---------+
//
// If the deadline actor determines that the deadline has expired, the socket
// is closed and any outstanding operations are consequently cancelled.
//
// Connection establishment involves trying each endpoint in turn until a
// connection is successful, or the available endpoints are exhausted. If the
// deadline actor closes the socket, the connect actor is woken up and moves to
// the next endpoint.
//
//  +---------------+
//  |               |
//  | start_connect |<---+
//  |               |    |
//  +---------------+    |
//           |           |
//  async_-  |    +----------------+
// connect() |    |                |
//           +--->| handle_connect |
//                |                |
//                +----------------+
//                          :
// Once a connection is     :
// made, the connect        :
// actor forks in two -     :
//                          :
// an actor for reading     :       and an actor for
// inbound messages:        :       sending writes:
//                          :
//  +------------+          :          +-------------+
//  |            |<- - - - -+- - - - ->|             |
//  | start_read |                     | start_write |<---+
//  |            |<---+                |             |    |
//  +------------+    |                +-------------+    | async_wait()
//          |         |                        |          |
//  async_- |    +-------------+       async_- |    +--------------+
//   read_- |    |             |       write() |    |              |
//  until() +--->| handle_read |               +--->| handle_write |
//               |             |                    |              |
//               +-------------+                    +--------------+
//
// The input actor reads messages from the socket, where messages are delimited
// by the newline character.
//

#include "HTTPClient.hpp"
#include "Log.h"
#include "ServerModule.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

// Connect Timeout in seconds after which we cancel the HTTP request
#define CLIENT_CONNECT_TIMEOUT_S 15
// Read Timeout in seconds after which we cancel the HTTP request
#define CLIENT_READ_TIMEOUT_S 300

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;
using namespace boost::iostreams;
using namespace std;

namespace synthese
{
	namespace server
	{

		HTTPClient::HTTPClient(boost::asio::io_service& io_service,
			string serverHost,
			string serverPort,
			const string& url,
			const string& postData,
			const string& contentType,
			const bool acceptGzip,
			string &payload)
		: stopped_(false),
		  socket_(io_service),
		  deadline_(io_service),
		  _serverHost(serverHost),
		  _serverPort(serverPort),
		  _url(url),
		  _postData(postData),
		  _contentType(contentType),
		  _acceptGzip(acceptGzip),
		  _gotHeader(false),
		  _payload(payload),
		  _announcedContentLength(0)
		{
		}

		// Called by the user of the client class to initiate the connection process.
		// The endpoint iterator will have been obtained using a tcp::resolver.
		void HTTPClient::start(tcp::resolver::iterator endpoint_iter)
		{
			// Start the connect actor.
			startConnect(endpoint_iter);

			// Start the deadline actor. You will note that we're not setting any
			// particular deadline here. Instead, the connect and input actors will
			// update the deadline prior to each asynchronous operation.
			deadline_.async_wait(boost::bind(&HTTPClient::checkDeadline, this));
		}

		// This function terminates all the actors to shut down the connection. It
		// may be called by the user of the client class, or by the class itself in
		// response to graceful termination or an unrecoverable error.
		void HTTPClient::stop()
		{
			stopped_ = true;
			socket_.close();
			deadline_.cancel();
		}

		void HTTPClient::startConnect(tcp::resolver::iterator endpoint_iter)
		{
			if (endpoint_iter != tcp::resolver::iterator())
			{
				util::Log::GetInstance().trace("HTTPClient, trying to connect to: " +
					boost::lexical_cast<string>(endpoint_iter->endpoint())
				);

				// Set a deadline for the connect operation.
				deadline_.expires_from_now(boost::posix_time::seconds(CLIENT_CONNECT_TIMEOUT_S));

				// Start the asynchronous connect operation.
				socket_.async_connect(endpoint_iter->endpoint(),
					  boost::bind(&HTTPClient::handleConnect,
								  this, _1, endpoint_iter)
				);
			}
			else
			{
				// There are no more endpoints to try. Shut down the client.
				stop();
			}
		}

		void HTTPClient::handleConnect(const boost::system::error_code& ec,
									   tcp::resolver::iterator endpoint_iter)
		{
			if (stopped_)
				return;

			// The async_connect() function automatically opens the socket at the start
			// of the asynchronous operation. If the socket is closed at this time then
			// the timeout handler must have run first.
			if (!socket_.is_open())
			{
				util::Log::GetInstance().warn("HTTPClient, connection timed out");

				// Try the next available endpoint.
				startConnect(++endpoint_iter);
			}

			// Check if the connect operation failed before the deadline expired.
			else if (ec)
			{
				_errorCode = ec;
				util::Log::GetInstance().error("HTTPClient, connection error: " +
					ec.message()
				);

				// We need to close the socket used in the previous connection attempt
				// before starting a new one.
				socket_.close();

				// Try the next available endpoint.
				startConnect(++endpoint_iter);
			}

			// Otherwise we have successfully established a connection.
			else
			{
				util::Log::GetInstance().trace("HTTPClient, connected to: " +
				   boost::lexical_cast<string>(endpoint_iter->endpoint())
				);

				// Form the request
				initRequest(_request);

				// Start the input actor.
				startRead();

				// Start the heartbeat actor.
				startWrite();
			}
		}

		/* Append the reception buffer to the payload buffer */
		void HTTPClient::payloadAppend()
		{
			size_t size(input_buffer_.size());
			const char* header = boost::asio::buffer_cast<const char*>(input_buffer_.data());
			_payload += string(header, size);
			input_buffer_.consume(size);
		}

		void HTTPClient::startRead()
		{
			// Set a deadline for the read operation.
			deadline_.expires_from_now(boost::posix_time::seconds(CLIENT_READ_TIMEOUT_S +
																  _request.size() / 10000));

			// Start an asynchronous operation to read a newline-delimited message.
			boost::asio::async_read_until(socket_, input_buffer_, '\n',
				boost::bind(&HTTPClient::handleRead, this, _1)
			);
		}

		void HTTPClient::handleRead(const boost::system::error_code& ec)
		{
			if (stopped_)
				return;

			if (!ec)
			{
				std::istream is(&input_buffer_);

				if(!_gotHeader)
				{
					// Extract the newline-delimited message from the buffer.
					std::string line;
					std::getline(is, line);
					// Create a map of each http header received
					line.erase(line.find_last_not_of("\n\r")+1);
					if(line.empty())
					{
						_gotHeader = true;
					}
					else
					{
						std::vector<std::string> strs;
						boost::split(strs, line, boost::is_any_of(": "),
							boost::algorithm::token_compress_on
						);
						if(strs.size() >= 2)
						{
							_headers[strs[0]] = strs[1];
							util::Log::GetInstance().trace("HTTPClient: received header: " +
								strs[0] + ": " + strs[1]
							);

							if(strs[0].substr(0, 5) == "HTTP/" && strs[1] != "200")
							{
								stop();
								throw Exception("Response returned with status code " + strs[1]);
							}
							else if(strs[0] == "Content-Length")
							{
								_announcedContentLength = boost::lexical_cast<long long>(strs[1]);
							}
						}
						else
						{
							util::Log::GetInstance().warn("HTTPClient: malformed header value: " +
								line
							);
						}
					}
				}
				else
				{
					payloadAppend();
				}

				startRead();
			}
			else
			{

				if(ec == boost::asio::error::eof)
				{
					// Save the last chunk of received data
					payloadAppend();

					// Check the announced length matches with what we got
					if(_payload.size() != _announcedContentLength)
					{
						_errorCode = boost::asio::error::broken_pipe;
						util::Log::GetInstance().error("HTTPClient: received payload size: " +
							boost::lexical_cast<string>(_payload.size()) +
							" instead of announced: " + boost::lexical_cast<string>(_announcedContentLength)
						);

						// Reset the partialy received databoost::lexical_cast<string>(_payload.size())
						_payload.clear();
					}
					else
					{
						util::Log::GetInstance().trace("HTTPClient: received payload size: " +
													   boost::lexical_cast<string>(_payload.size())
						);

						// If the header indicates a zipped stream dezip it
						Headers::const_iterator it(
							_headers.find("Content-Encoding")
						);
						if(	it != _headers.end() &&
							it->second == "gzip" &&
							!_payload.empty()
						){
							ostringstream os;
							istringstream tmp(_payload);
							filtering_stream<input> fs;
							fs.push(gzip_decompressor());
							fs.push(tmp);
							boost::iostreams::copy(fs, os);
							_payload = os.str();
						}

					}
				}
				else
				{
					_errorCode = ec;
					util::Log::GetInstance().error("HTTPClient: error on receive: " +
						ec.message()
					);
				}

				stop();
			}
		}

		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		void HTTPClient::initRequest(boost::asio::streambuf &request)
		{
			std::ostream request_stream(&request);
			if(_postData.empty())
			{
				request_stream << "GET";
			}
			else
			{
				request_stream << "POST";
			}
			// TODO : upgrade to 1.1 by handling Transfer-Encoding: chunked
			request_stream << " " << _url << " HTTP/1.0\r\n";
			request_stream << "Host: " << _serverHost << ":" << _serverPort << "\r\n";
			request_stream << "User-Agent: SYNTHESE/" << ServerModule::VERSION << "\r\n";
			request_stream << "Accept: */*\r\n";
			if(_acceptGzip)
			{
				request_stream << "Accept-Encoding: gzip\r\n";
			}
			if(!_postData.empty())
			{
				request_stream << "Content-Length: " << _postData.size() << "\r\n";
			}
			if(!_contentType.empty())
			{
				request_stream << "Content-Type: " << _contentType << "\r\n";
			}
			request_stream << "Connection: close\r\n\r\n";
			if(!_postData.empty())
			{
				request_stream << _postData;
			}
		}

		void HTTPClient::startWrite()
		{
			if (stopped_)
				return;

			// Start an asynchronous operation to send the http request
			boost::asio::async_write(socket_, _request,
				boost::bind(&HTTPClient::handleWrite, this, _1)
			);
		}

		void HTTPClient::handleWrite(const boost::system::error_code& ec)
		{
			if (stopped_)
				return;

			if (ec)
			{
				_errorCode = ec;
				util::Log::GetInstance().error("HTTPClient: failed to send the request: " +
					ec.message()
				);
				stop();
			}
		}

		void HTTPClient::checkDeadline()
		{
			if (stopped_)
				return;

			// Check whether the deadline has passed. We compare the deadline against
			// the current time since a new asynchronous operation may have moved the
			// deadline before this actor had a chance to run.
			if (deadline_.expires_at() <= deadline_timer::traits_type::now())
			{
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are cancelled.
				socket_.close();

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				deadline_.expires_at(boost::posix_time::pos_infin);
			}

			// Put the actor back to sleep.
			deadline_.async_wait(boost::bind(&HTTPClient::checkDeadline, this));
		}

		// Returns the last error code
		boost::system::error_code HTTPClient::getErrorCode()
		{
			return _errorCode;
		}

}	}

