
#include "BasicClient.h"

#include "Exception.h"
#include "ServerModule.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::iostreams;
using boost::asio::ip::tcp;

namespace synthese
{
	namespace server
	{
		BasicClient::BasicClient(
			const std::string& serverHost,
			const string serverPort,
			int timeOut,
			bool outputHTTPHeaders,
			bool acceptGzip
		):	_serverHost (serverHost),
			_serverPort (serverPort),
			_timeOut (timeOut),
			_outputHTTPHeaders(outputHTTPHeaders),
			_acceptGzip(acceptGzip)
		{}



		string BasicClient::get(
			const std::string& url
		) const {
			string fakePostData;
			string fakeContentType;
			return _send(url, fakePostData, fakeContentType);
		}



		string BasicClient::post(
			const std::string& url,
			const std::string& data,
			string contentType
		) const	{
			return _send(url, data, contentType);
		}



		string BasicClient::_send(
			const std::string& url,
			const std::string& postData,
			const string& contentType
		) const	{
			try
			{
				boost::asio::io_service io_service;

				// Get a list of endpoints corresponding to the server name.
				tcp::resolver resolver(io_service);
				tcp::resolver::query query(_serverHost, _serverPort);
				tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
				tcp::resolver::iterator end;

				// Try each endpoint until we successfully establish a connection.
				tcp::socket socket(io_service);
				boost::system::error_code error = boost::asio::error::host_not_found;
				while (error && endpoint_iterator != end)
				{
					socket.close();
					socket.connect(*endpoint_iterator++, error);
				}
				if (error)
					throw boost::system::system_error(error);

				// Form the request. We specify the "Connection: close" header so that the
				// server will close the socket after transmitting the response. This will
				// allow us to treat all data up until the EOF as the content.
				boost::asio::streambuf request;
				std::ostream request_stream(&request);
				if(postData.empty())
				{
					request_stream << "GET";
				}
				else
				{
					request_stream << "POST";
				}
				request_stream << " " << url << " HTTP/1.1\r\n";
				request_stream << "Host: " << _serverHost << ":" << _serverPort << "\r\n";
				request_stream << "User-Agent: SYNTHESE/" << ServerModule::VERSION << "\r\n";
				request_stream << "Accept: */*\r\n";
				if(_acceptGzip)
				{
					request_stream << "Accept-Encoding: gzip\r\n";
				}
				if(!postData.empty())
				{
					request_stream << "Content-Length: " << postData.size() << "\r\n";
				}
				if(!contentType.empty())
				{
					request_stream << "Content-Type: " << contentType << "\r\n";
				}
				request_stream << "Connection: keep-alive\r\n\r\n";
				if(!postData.empty())
				{
					request_stream << postData;
				}

				// Send the request.
				boost::asio::write(socket, request);

				// Read the response status line.
				boost::asio::streambuf response;
				boost::asio::read_until(socket, response, "\r\n");

				// Check that response is OK.
				std::istream response_stream(&response);
				std::string http_version;
				response_stream >> http_version;
				unsigned int status_code;
				response_stream >> status_code;
				std::string status_message;
				std::getline(response_stream, status_message);
				if (!response_stream || http_version.substr(0, 5) != "HTTP/")
				{
					throw Exception("Invalid response");
				}
				if (status_code != 200)
				{
					throw Exception("Response returned with status code " + status_code);
				}

				// Read the response headers, which are terminated by a blank line.
				boost::asio::read_until(socket, response, "\r\n\r\n");

				stringstream tmp;

				// Process the response headers.
				std::string header;
				typedef map<string, string> Headers;
				Headers headers;
				while (std::getline(response_stream, header) && header != "\r")
				{
					if(header[header.size()-1] == '\r')
					{
						header = header.substr(0, header.size()-1);
					}

					if(!header.empty())
					{
						size_t p(1);
						for(; p<header.size() && header[p] != ':'; ++p) ; 
						if(p != header.size())
						{
							headers.insert(
								make_pair(
									trim_copy(header.substr(0, p)),
									trim_copy(header.substr(p+2))
							)	);
						}
					}
				}

				// Output HTTP Headers
				if(_outputHTTPHeaders)
				{
					BOOST_FOREACH(const Headers::value_type& it, headers)
					{
						tmp << it.first << ": " << it.second << "\r\n";
					}
				}

				// Write whatever content we already have to output.
				if (response.size() > 0)
				{
					tmp << &response;
				}

				// Read until EOF, writing data to output as we go.
				while(asio::read(socket, response, asio::transfer_at_least(1), error))
				{
					tmp << &response;
				}
				if (error != asio::error::eof)
				{
					throw system::system_error(error);
				}

				// Output with or without gzip compression
				Headers::const_iterator it(
					headers.find("Content-Encoding")
				);
				if(	it != headers.end() &&
					it->second == "gzip"
				){
					stringstream os;
					filtering_stream<input> fs;
					fs.push(gzip_decompressor());
					fs.push(os);
					boost::iostreams::copy(tmp, os);
					return os.str();
				}
				else
				{
					return tmp.str();
				}
			}
			catch (std::exception& e)
			{
				throw Exception(e.what());
			}
		}
}	}
