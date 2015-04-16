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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/function.hpp>

namespace synthese
{
	namespace
	{
		bool str_iless(std::string const & a, std::string const & b)
		{
			return boost::algorithm::lexicographical_compare(a, b, boost::is_iless());
		}
	}



	namespace server
	{
		/// addtogroup m15
		/// @{

		/// A request received from a client.
		struct HTTPRequest
		{
			/// Case insensitive map for storing the headers to match the HTTP spec.
			typedef std::map<
				std::string,
				std::string,
				boost::function<bool(std::string const &, std::string const &)> > Headers;
			typedef Headers::value_type Header;

			std::string method;
			std::string uri;
			std::string ipaddr;
			int http_version_major;
			int http_version_minor;
			Headers headers;
			std::string postData;

			HTTPRequest() : headers(&str_iless)
			{
			}
		};

		/// @}

	} // namespace server
} // namespace http

#endif // HTTP_SERVER3_REQUEST_HPP
