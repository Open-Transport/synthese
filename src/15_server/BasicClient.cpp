
#include "BasicClient.h"
#include "Exception.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;

namespace synthese
{
	using namespace util;
	
	namespace server
	{


		BasicClient::BasicClient(
			const std::string& serverHost,
			const string serverPort,
			int timeOut,
			bool outputHTTPHeaders
		):	_serverHost (serverHost),
			_serverPort (serverPort),
			_timeOut (timeOut),
			_outputHTTPHeaders(outputHTTPHeaders)
		{
		}



	    
		void BasicClient::request(
			std::ostream& out,
			const std::string& url
		){
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
				request_stream << "GET " << url << " HTTP/1.0\r\n";
				request_stream << "Host: " << _serverHost << "\r\n";
				request_stream << "Accept: */*\r\n";
				request_stream << "Connection: close\r\n\r\n";

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

				// Process the response headers.
				std::string header;
				while (std::getline(response_stream, header) && header != "\r")
				{
					if(_outputHTTPHeaders)
					{
						out << header << "\n";
					}
				}
				if(_outputHTTPHeaders)
				{
					out << "\n";
				}

				// Write whatever content we already have to output.
				if (response.size() > 0)
					out << &response;

				// Read until EOF, writing data to output as we go.
				while (boost::asio::read(socket, response,
					boost::asio::transfer_at_least(1), error))
					out << &response;
				if (error != boost::asio::error::eof)
					throw boost::system::system_error(error);
			}
			catch (std::exception& e)
			{
				throw Exception(e.what());
			}
		}
	}
}
