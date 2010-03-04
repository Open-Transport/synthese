//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_REQUEST_HPP
#define HTTP_SERVER3_REQUEST_HPP

#include <string>
#include <map>

namespace synthese
{
	namespace server
	{
		/// addtogroup m15
		/// @{

		/// A request received from a client.
		struct HTTPRequest
		{
			typedef std::map<std::string, std::string> Headers;
			typedef Headers::value_type Header;

			std::string method;
			std::string uri;
			std::string ipaddr;
			int http_version_major;
			int http_version_minor;
			Headers headers;
			std::string postData;
		};

		/// @}

	} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_REQUEST_HPP
