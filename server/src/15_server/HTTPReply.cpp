//
// HTTPReply.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "HTTPReply.hpp"
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	namespace server
	{

		namespace status_strings {

		const std::string ok =
		  "HTTP/1.0 200 OK\r\n";
		const std::string created =
		  "HTTP/1.0 201 Created\r\n";
		const std::string accepted =
		  "HTTP/1.0 202 Accepted\r\n";
		const std::string no_content =
		  "HTTP/1.0 204 No Content\r\n";
		const std::string multiple_choices =
		  "HTTP/1.0 300 Multiple Choices\r\n";
		const std::string moved_permanently =
		  "HTTP/1.0 301 Moved Permanently\r\n";
		const std::string moved_temporarily =
		  "HTTP/1.0 302 Moved Temporarily\r\n";
		const std::string not_modified =
		  "HTTP/1.0 304 Not Modified\r\n";
		const std::string bad_request =
		  "HTTP/1.0 400 Bad Request\r\n";
		const std::string unauthorized =
		  "HTTP/1.0 401 Unauthorized\r\n";
		const std::string forbidden =
		  "HTTP/1.0 403 Forbidden\r\n";
		const std::string not_found =
		  "HTTP/1.0 404 Not Found\r\n";
		const std::string internal_server_error =
		  "HTTP/1.0 500 Internal Server Error\r\n";
		const std::string not_implemented =
		  "HTTP/1.0 501 Not Implemented\r\n";
		const std::string bad_gateway =
		  "HTTP/1.0 502 Bad Gateway\r\n";
		const std::string service_unavailable =
		  "HTTP/1.0 503 Service Unavailable\r\n";

		boost::asio::const_buffer to_buffer(HTTPReply::status_type status)
		{
		  switch (status)
		  {
		  case HTTPReply::ok:
			return boost::asio::buffer(ok);
		  case HTTPReply::created:
			return boost::asio::buffer(created);
		  case HTTPReply::accepted:
			return boost::asio::buffer(accepted);
		  case HTTPReply::no_content:
			return boost::asio::buffer(no_content);
		  case HTTPReply::multiple_choices:
			return boost::asio::buffer(multiple_choices);
		  case HTTPReply::moved_permanently:
			return boost::asio::buffer(moved_permanently);
		  case HTTPReply::moved_temporarily:
			return boost::asio::buffer(moved_temporarily);
		  case HTTPReply::not_modified:
			return boost::asio::buffer(not_modified);
		  case HTTPReply::bad_request:
			return boost::asio::buffer(bad_request);
		  case HTTPReply::unauthorized:
			return boost::asio::buffer(unauthorized);
		  case HTTPReply::forbidden:
			return boost::asio::buffer(forbidden);
		  case HTTPReply::not_found:
			return boost::asio::buffer(not_found);
		  case HTTPReply::internal_server_error:
			return boost::asio::buffer(internal_server_error);
		  case HTTPReply::not_implemented:
			return boost::asio::buffer(not_implemented);
		  case HTTPReply::bad_gateway:
			return boost::asio::buffer(bad_gateway);
		  case HTTPReply::service_unavailable:
			return boost::asio::buffer(service_unavailable);
		  default:
			return boost::asio::buffer(internal_server_error);
		  }
		}

		} // namespace status_strings

		namespace misc_strings {

		const char name_value_separator[] = { ':', ' ' };
		const char crlf[] = { '\r', '\n' };

		} // namespace misc_strings

		std::vector<boost::asio::const_buffer> HTTPReply::to_buffers()
		{
		  std::vector<boost::asio::const_buffer> buffers;
		  buffers.push_back(status_strings::to_buffer(status));
		  BOOST_FOREACH(const Header& h, headers)
		  {
			buffers.push_back(boost::asio::buffer(h.first));
			buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
			buffers.push_back(boost::asio::buffer(h.second));
			buffers.push_back(boost::asio::buffer(misc_strings::crlf));
		  }
		  buffers.push_back(boost::asio::buffer(misc_strings::crlf));
		  buffers.push_back(boost::asio::buffer(content));
		  return buffers;
		}

		namespace stock_replies {

		std::string build_reply(
			HTTPReply::status_type status, 
			const std::string &title,
			const std::string &description
		)
		{
			std::ostringstream oss;
			oss	<< "<html><head><title>" << title << "</title></head>"
				<< "<body>"
				<< "<h1>" << status << " " << title << "</h1>"
				<< "<h3>" << description << "</h3>"
				<< "</body></html>";
			return oss.str();
		}

		std::string to_string(
			HTTPReply::status_type status,
			const std::string &description
		)
		{
		  switch (status)
		  {
		  case HTTPReply::ok:
			return build_reply(status, "Ok", description);
		  case HTTPReply::created:
			return build_reply(status, "Created", description);
		  case HTTPReply::accepted:
			return build_reply(status, "Accepted", description);
		  case HTTPReply::no_content:
			return build_reply(status, "No Content", description);
		  case HTTPReply::multiple_choices:
			return build_reply(status, "Multiple Choices", description);
		  case HTTPReply::moved_permanently:
			return build_reply(status, "Moved Permanently", description);
		  case HTTPReply::moved_temporarily:
			return build_reply(status, "Moved Temporarily", description);
		  case HTTPReply::not_modified:
			return build_reply(status, "Not Modified", description);
		  case HTTPReply::bad_request:
			return build_reply(status, "Bad Request", description);
		  case HTTPReply::unauthorized:
			return build_reply(status, "Unauthorized", description);
		  case HTTPReply::forbidden:
			return build_reply(status, "Forbidden", description);
		  case HTTPReply::not_found:
			return build_reply(status, "Not Found", description);
		  case HTTPReply::internal_server_error:
			return build_reply(status, "Internal Server Error", description);
		  case HTTPReply::not_implemented:
			return build_reply(status, "Not Implemented", description);
		  case HTTPReply::bad_gateway:
			return build_reply(status, "Bad Gateway", description);
		  case HTTPReply::service_unavailable:
			return build_reply(status, "Service Unavailable", description);
		  default:
			return build_reply(status, "Internal Server Error", description);
		  }
		}

		} // namespace stock_replies

		HTTPReply HTTPReply::stock_reply(
			HTTPReply::status_type status,
			const std::string &description
		)
		{
		  HTTPReply rep;
		  rep.status = status;
		  rep.content = stock_replies::to_string(status, description);
		  rep.headers.insert(make_pair("Content-Length", boost::lexical_cast<std::string>(rep.content.size())));
		  rep.headers.insert(make_pair("Content-Type", "text/html"));
		  return rep;
		}

	} // namespace server
} // namespace http
