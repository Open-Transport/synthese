/** PocoHttpServer class implementation.
	@file PocoHttpServer.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "PocoHttpServer.h"
#include "Log.h"
#include "15_server/version.h"
#include "Request.h"
#include "DynamicRequest.h"
#include "Function.h"
#include "HTTPRequest.hpp"
#include "RequestException.h"
#include "ActionException.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>


using namespace boost::posix_time;
using namespace std;
using namespace boost::algorithm;
using namespace boost::iostreams;


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPCookie.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/StreamCopier.h"
#include <iostream>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPCookie;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::MessageHeader;
using Poco::Net::NameValueCollection;
using Poco::StreamCopier;

using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace server
	{

		// A request handler to test the raw performance of Poco before calling
		// SYNTHESE services
		class PocoTestRequestHandler: public HTTPRequestHandler
		{
		public:
			PocoTestRequestHandler()
			{
			}

			void handleRequest(HTTPServerRequest& request,
							   HTTPServerResponse& response)
			{
				response.setChunkedTransferEncoding(true);
				response.setContentType("text/html");

				std::ostream& ostr = response.send();
				ostr << "<html><head><title>HTTPTimeServer powered by POCO C++ Libraries</title>";
				ostr << "<meta http-equiv=\"refresh\" content=\"1\"></head>";
				ostr << "<body><p style=\"text-align: center; font-size: 48px;\">";
				ostr << "Poco Raw Performance Test";
				ostr << "</p></body></html>";
			}
		};

		/// This class handles all client connections.
		class RequestHandler: public HTTPRequestHandler
		{
		public:
			RequestHandler()
			{
			}

			void _setCookieHeaders(HTTPServerResponse& response,
			   const CookiesMap& cookiesMap
			)
			{
				BOOST_FOREACH(const CookiesMap::value_type &cookie, cookiesMap)
				{
					HTTPCookie httpCookie(cookie.first, cookie.second.first);
					httpCookie.setMaxAge(cookie.second.second);
					response.addCookie(httpCookie);
					Log::GetInstance ().debug ("add cookie " + cookie.first + "=" +  cookie.second.first);
				}
			}

			void handleRequest(HTTPServerRequest& request,
							   HTTPServerResponse& response)
			{
				Log::GetInstance ().debug ("Received request : " +
					request.getURI() +
					" (" + boost::lexical_cast<string>(request.getURI().size()) + " bytes)"
				);

				// GZip compression ?
				bool gzipCompression(false);
				{
					set<string> formats;
					split(formats, request["Accept-Encoding"], is_any_of(","));
					gzipCompression = (formats.find("gzip") != formats.end());
				}

				// Request run
				try {
					DynamicRequest dynamicRequest(request, response);

					stringstream serviceOutStream;

					// Never compress for localhost use
					if(gzipCompression &&
					   request.clientAddress().host().toString() != "127.0.0.1"
					)
					{
						filtering_stream<output> fs;
						fs.push(gzip_compressor());
						fs.push(serviceOutStream);
						dynamicRequest.run(fs);
						fs.pop();
					}
					else
					{
						gzipCompression = false;
						dynamicRequest.run(serviceOutStream);
					}

					response.setChunkedTransferEncoding(false);
					response.setContentType(dynamicRequest.getOutputMimeType());
					response.setContentLength(serviceOutStream.str().length());
					response.setTransferEncoding("utf-8");
					if(gzipCompression)
					{
						response.add("Content-Encoding", "gzip");
					}

					std::ostream& ostr = response.send();

					StreamCopier::copyStream(serviceOutStream, ostr);
				}
				catch(Request::RedirectException& e)
				{
					/* FIXME Not supported on Poco 1.3.6 but exist on 1.4.6
					response.redirect(
						e.getLocation(),
						e.getPermanently() ? HTTPResponse::HTTP_MOVED_PERMANENTLY : HTTPResponse::HTTP_FOUND
					); */
					_setCookieHeaders(response, e.getCookiesMap());
					response.setChunkedTransferEncoding(false);
					response.setStatus(e.getPermanently() ?
						HTTPResponse::HTTP_MOVED_PERMANENTLY : HTTPResponse::HTTP_FOUND
					);
					response.redirect(e.getLocation());
				}
				catch(Request::ForbiddenRequestException&)
				{
					Log::GetInstance().debug("Forbidden request");
					response.setChunkedTransferEncoding(false);
					response.setStatus(HTTPResponse::HTTP_FORBIDDEN);
					response.send();
				}
				catch(Request::NotFoundException&)
				{
					Log::GetInstance().debug("Path not found");
					response.setChunkedTransferEncoding(false);
					response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
					response.send();
				}
				catch (RequestException& e)
				{
					Log::GetInstance().debug("Request error", e);
					response.setChunkedTransferEncoding(false);
					response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
					response.send();
				}
				catch (ActionException& e)
				{
					Log::GetInstance().debug("Action error", e);
					response.setChunkedTransferEncoding(false);
					response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
					response.send();
				}
				catch(Exception& e)
				{
					Log::GetInstance().debug("Exception", e);
					response.setChunkedTransferEncoding(false);
					response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
					response.send();
				}
				catch(std::exception& e)
				{
					Log::GetInstance().debug("An unhandled exception has occured : " + std::string (e.what ()));
					response.setChunkedTransferEncoding(false);
					response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
					response.send();
				}
				catch(...)
				{
					Log::GetInstance().debug("An unhandled exception has occured.");
					response.setChunkedTransferEncoding(false);
					response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
					response.send();
				}
			}
		};

		class RequestHandlerFactory: public HTTPRequestHandlerFactory
		{
		public:
			RequestHandlerFactory()
			{
			}

			HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
			{
				if(request.getURI() == "/_poco_test")
					return new PocoTestRequestHandler();
				else
					return new RequestHandler();
			}
		};


		PocoHttpServer::PocoHttpServer(unsigned short port):
			_port(port)
		{
		}

		PocoHttpServer::~PocoHttpServer()
		{
		}

		void PocoHttpServer::initialize(Application& self)
		{
			ServerApplication::initialize(self);
		}

		void PocoHttpServer::uninitialize()
		{
			ServerApplication::uninitialize();
		}

		int PocoHttpServer::main(const std::vector<std::string>& args)
		{
			int maxQueued  = 100;
			int maxThreads = 16;
			ThreadPool::defaultPool().addCapacity(maxThreads);

			HTTPServerParams* pParams = new HTTPServerParams;
			pParams->setMaxQueued(maxQueued);
			pParams->setMaxThreads(maxThreads);

			// set-up a server socket
			ServerSocket svs(_port);
			// set-up a HTTPServer instance
			HTTPServer srv(new RequestHandlerFactory(), svs, pParams);
			// start the HTTPServer
			srv.start();
			// wait for CTRL-C or kill
			waitForTerminationRequest();
			// Stop the HTTPServer
			srv.stop();

			return Application::EXIT_OK;
		}
	}
}

