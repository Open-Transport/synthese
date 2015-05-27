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

#include <string>

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
			_connectionTimeout (timeOut == 0 ? boost::none : boost::optional<int>(timeOut)),
			_readTimeout (timeOut == 0 ? boost::none : boost::optional<int>(timeOut)),
			_outputHTTPHeaders(outputHTTPHeaders),
			_acceptGzip(acceptGzip)
		{}



		BasicClient::BasicClient(
			const std::string& serverHost,
			const std::string serverPort,
			boost::optional<int> connectionTimeout,
			boost::optional<int> readTimeout,
			bool outputHTTPHeaders,
			bool acceptGzip
		) : _serverHost (serverHost),
			_serverPort (serverPort),
			_connectionTimeout (connectionTimeout),
			_readTimeout (readTimeout),
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
				result, /* out value */
				_connectionTimeout, _readTimeout
			);

			client.start(r.resolve(tcp::resolver::query(_serverHost, _serverPort)));

			io_service.run();

			if(client.getErrorCode())
			{
				throw boost::system::system_error(client.getErrorCode());
			}

			return result;
		}


		BasicClient::Uri BasicClient::Uri::parseUri(const std::string url)
		{
			Uri result;
			result.port = "80";
			result.path = "/";
			size_t pos(0);
			for(; pos+1<url.size() && (url[pos] != '/' || url[pos+1] != '/'); ++pos) ;
			if(pos + 1 == url.size())
			{
				throw Exception("Bad URL");
			}
			size_t pos2(pos+2);
			for(; pos2<url.size() && url[pos2]!=':' && url[pos2]!='/'; ++pos2) ;
			result.host = url.substr(pos+2, pos2 - pos - 2);
			if(pos2 < url.size())
			{
				if(url[pos2] == ':')
				{
					++pos2;
					pos = pos2;
					for(; pos2<url.size() && url[pos2]!='/'; ++pos2) ;
					result.port = url.substr(pos, pos2-pos);
				}
				result.path = url.substr(pos2);
			}

			return result;
		}



		std::string BasicClient::Get( const std::string url )
		{
			Uri parsedUri = Uri::parseUri(url);
			BasicClient c(parsedUri.host, parsedUri.port);
			return c.get(parsedUri.path);
		}
}	}

#endif
