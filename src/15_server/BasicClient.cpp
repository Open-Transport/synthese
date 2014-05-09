// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
#ifndef WIN32

#include "BasicClient.h"
#include "Exception.h"
#include "HTTPClient.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::asio;
using namespace std;

// Timeout in seconds after which we cancel the HTTP request
#define CLIENT_TIMEOUT_S 15

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
			string result;
			boost::asio::io_service io_service;
			tcp::resolver r(io_service);

			HTTPClient client(io_service, _serverHost, _serverPort,
				url, postData, contentType, _acceptGzip,
				result /* out value */
			);

			client.start(r.resolve(tcp::resolver::query(_serverHost, _serverPort)));

			io_service.run();

			if(client.getErrorCode())
			{
				throw boost::system::system_error(client.getErrorCode());
			}

			return result;
		}


		std::string BasicClient::Get( const std::string url )
		{
			// Parsing of the url
			string host;
			string port("80");
			string path("/");
			size_t pos(0);
			for(; pos+1<url.size() && (url[pos] != '/' || url[pos+1] != '/'); ++pos) ;
			if(pos + 1 == url.size())
			{
				throw Exception("Bad URL");
			}
			size_t pos2(pos+2);
			for(; pos2<url.size() && url[pos2]!=':' && url[pos2]!='/'; ++pos2) ;
			host = url.substr(pos+2, pos2 - pos - 2);
			if(pos2 < url.size())
			{
				if(url[pos2] == ':')
				{
					++pos2;
					pos = pos2;
					for(; pos2<url.size() && url[pos2]!='/'; ++pos2) ;
					port = url.substr(pos, pos2-pos);
				}
				path = url.substr(pos2);
			}

			BasicClient c(host, port);
			return c.get(path);
		}
}	}

#endif
