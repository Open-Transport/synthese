//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "HTTPRequestHandler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "HTTPReply.hpp"
#include "HTTPRequest.hpp"
#include "Log.h"
#include "Request.h"
#include "RequestException.h"
#include "ActionException.h"

using namespace boost;
using namespace std;

namespace synthese
{
	namespace server
	{
		using namespace util;

		HTTPRequestHandler::HTTPRequestHandler()
		{
		}

		void HTTPRequestHandler::handle_request(const HTTPRequest& req, HTTPReply& rep)
		{
		  // Decode url to path.
		  std::string request_path;
		  if (!url_decode(req.uri, request_path))
		  {
			rep = HTTPReply::stock_reply(HTTPReply::bad_request);
			return;
		  }

		  Log::GetInstance ().debug ("Received request : " + 
			  req.uri + " (" + lexical_cast<string>(req.uri.size()) + " bytes)");

		  try
		  {
			  Request request(req);
			  stringstream output;
			  request.run(output);
			  rep.status = HTTPReply::ok;
			  rep.content.append(output.str());
			  rep.headers[0].name = "Content-Length";
			  rep.headers[0].value = lexical_cast<string>(rep.content.size());
			  rep.headers[1].name = "Content-Type";
			  rep.headers[1].value = request.getOutputMimeType();
		  }
		  catch (RequestException& e)
		  {
			  Log::GetInstance().debug("Request error", e);
			  rep = HTTPReply::stock_reply(HTTPReply::bad_request);
		  }
		  catch (ActionException& e)
		  {
			  Log::GetInstance().debug("Action error", e);
			  rep = HTTPReply::stock_reply(HTTPReply::bad_request);
		  }
		  catch(util::Exception& e)
		  {
			  Log::GetInstance().debug("Exception", e);
			  rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
		  }
		  catch(std::exception& e)
		  {
			  Log::GetInstance().debug("An unhandled exception has occured : " + std::string (e.what ()));
			  rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
		  }
		  catch(...)
		  {
			  Log::GetInstance().debug("An unhandled exception has occured.");
			  rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
		  }
		}



		bool HTTPRequestHandler::url_decode(const std::string& in, std::string& out)
		{
		  out.clear();
		  out.reserve(in.size());
		  for (std::size_t i = 0; i < in.size(); ++i)
		  {
			if (in[i] == '%')
			{
			  if (i + 3 <= in.size())
			  {
				int value;
				std::istringstream is(in.substr(i + 1, 2));
				if (is >> std::hex >> value)
				{
				  out += static_cast<char>(value);
				  i += 2;
				}
				else
				{
				  return false;
				}
			  }
			  else
			  {
				return false;
			  }
			}
			else if (in[i] == '+')
			{
			  out += ' ';
			}
			else
			{
			  out += in[i];
			}
		  }
		  return true;
		}

	} // namespace server3
} // namespace http
