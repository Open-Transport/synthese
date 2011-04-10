
/** ServerModule class implementation.
    @file ServerModule.cpp

    This file belongs to the SYNTHESE project (public transportation specialized software)
    Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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


#include "ServerModule.h"
#include "EMail.h"
#include "Log.h"

#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#ifdef UNIX
  #define DEFAULT_TEMP_DIR "/tmp"
#endif
#ifdef WIN32
  #define DEFAULT_TEMP_DIR "c:/temp"
#endif

#include "HTTPReply.hpp"
#include "HTTPRequest.hpp"
#include "Log.h"
#include "DynamicRequest.h"
#include "Function.h"
#include "RequestException.h"
#include "ActionException.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	
	template<> const std::string util::FactorableTemplate<ModuleClass, ServerModule>::FACTORY_KEY("15_server");

    namespace server
    {
		ServerModule::SessionMap	ServerModule::_sessionMap;
		boost::asio::io_service ServerModule::_io_service;
		boost::asio::ip::tcp::acceptor ServerModule::_acceptor(ServerModule::_io_service);
		connection_ptr ServerModule::_new_connection(new HTTPConnection(ServerModule::_io_service));
		ServerModule::Threads ServerModule::_threads;
		size_t ServerModule::_waitingThreads(0);
		recursive_mutex ServerModule::_threadManagementMutex;
		time_duration ServerModule::_sessionMaxDuration(minutes(30));


		const string ServerModule::MODULE_PARAM_PORT ("port");
		const string ServerModule::MODULE_PARAM_NB_THREADS ("nb_threads");
		const string ServerModule::MODULE_PARAM_LOG_LEVEL ("log_level");
		const string ServerModule::MODULE_PARAM_SMTP_SERVER ("smtp_server");
		const string ServerModule::MODULE_PARAM_SMTP_PORT ("smtp_port");
		const string ServerModule::MODULE_PARAM_SESSION_MAX_DURATION("session_max_duration");

		const std::string ServerModule::VERSION("3.3.0");

		template<> const string ModuleClassTemplate<ServerModule>::NAME("Server kernel");


		template<> void ModuleClassTemplate<ServerModule>::PreInit()
		{
			RegisterParameter(ServerModule::MODULE_PARAM_PORT, "8080", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_NB_THREADS, "5", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_LOG_LEVEL, "1", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_SMTP_SERVER, "smtp", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_SMTP_PORT, "mail", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_SESSION_MAX_DURATION, "30", &ServerModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<ServerModule>::Init()
		{
			try 
			{
				// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
				string address("0.0.0.0");
				string port(GetParameter(ServerModule::MODULE_PARAM_PORT));
				
				asio::ip::tcp::resolver resolver(ServerModule::_io_service);
				asio::ip::tcp::resolver::query query(address, port);
				asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
				
				ServerModule::_acceptor.open(endpoint.protocol());
				ServerModule::_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
				ServerModule::_acceptor.bind(endpoint);
				ServerModule::_acceptor.listen();
				ServerModule::_acceptor.async_accept(
					ServerModule::_new_connection->socket(),
					bind(&ServerModule::HandleAccept, asio::placeholders::error)
				);
			}
			
			catch (std::exception& ex)
			{
				Log::GetInstance ().fatal ("Unexpected exception", ex);
			} 
			catch (...)
			{
				Log::GetInstance ().fatal ("Unexpected exception");
			}
		}


		void ServerModule::RunHTTPServer()
		{
			// Create a pool of threads to run all of the io_services.
			ServerModule::KillAllHTTPThreads();
	
			Log::GetInstance().info(
				"HTTP Server is now listening on port " + GetParameter(ServerModule::MODULE_PARAM_PORT) +
				" with at least "+ GetParameter(ServerModule::MODULE_PARAM_NB_THREADS) + " threads ..."
			);
		}


		template<> void ModuleClassTemplate<ServerModule>::End()
		{
			UnregisterParameter(ServerModule::MODULE_PARAM_PORT);
			UnregisterParameter(ServerModule::MODULE_PARAM_NB_THREADS);
			UnregisterParameter(ServerModule::MODULE_PARAM_LOG_LEVEL);
			UnregisterParameter(ServerModule::MODULE_PARAM_SMTP_SERVER);
			UnregisterParameter(ServerModule::MODULE_PARAM_SMTP_PORT);
			UnregisterParameter(ServerModule::MODULE_PARAM_SESSION_MAX_DURATION);

			ServerModule::_io_service.stop();
		}
		

		ServerModule::SessionMap& ServerModule::getSessions()
		{
			return _sessionMap;
		}


		void ServerModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
			if (name == MODULE_PARAM_PORT) 
			{
				// TODO : close and reopen service on the new port
			}
			if (name == MODULE_PARAM_LOG_LEVEL) 
			{
				Log::GetInstance ().setLevel (static_cast<Log::Level>(lexical_cast<int>(value)));
			}
			if(name == MODULE_PARAM_NB_THREADS)
			{
				
			}
			if(name == MODULE_PARAM_SESSION_MAX_DURATION)
			{
				_sessionMaxDuration = minutes(lexical_cast<int>(value));
			}
		}
		
		
		void ServerModule::HandleAccept(
			const boost::system::error_code& e
		){
			if (!e)
			{
				_new_connection->start();
				_new_connection.reset(new HTTPConnection(_io_service));
				_acceptor.async_accept(
					_new_connection->socket(),
					boost::bind(
						&ServerModule::HandleAccept,
						boost::asio::placeholders::error
					)
				);
			}
		}


		void ServerModule::HandleRequest(
			const HTTPRequest& req,
			HTTPReply& rep
		){
			try
			{
				Log::GetInstance ().debug ("Received request : " + 
					req.uri + " (" + lexical_cast<string>(req.uri.size()) + " bytes)" + (req.postData.empty() ? string() : " + "+ lexical_cast<string>(req.postData.size()) +" bytes of POST data : "+ req.postData.substr(0, 1000) ) );

				SetCurrentThreadAnalysing(req.uri + (req.postData.empty() ? string() : " + "+ req.postData.substr(0, 100)));
				DynamicRequest request(req);

				stringstream output;
				request.run(output);
				rep.status = HTTPReply::ok;
				rep.content.append(output.str());
				rep.headers.insert(make_pair("Content-Length", lexical_cast<string>(rep.content.size())));
				rep.headers.insert(make_pair("Content-Type", request.getOutputMimeType()));
				if(request.getFunction().get() && !request.getFunction()->getFileName().empty())
				{
					rep.headers.insert(make_pair("Content-Disposition", "attachement; filename="+ request.getFunction()->getFileName()));
				}

				_SetCookieHeaders(rep, request.getCookiesMap());
			}
			catch(Request::RedirectException& e)
			{
				rep = HTTPReply::stock_reply(e.getPermanently() ? HTTPReply::moved_permanently : HTTPReply::moved_temporarily);
				rep.headers.insert(make_pair("Location", e.getLocation()));
				_SetCookieHeaders(rep, e.getCookiesMap());
			}
			catch(Request::ForbiddenRequestException& e)
			{
				Log::GetInstance().debug("Forbidden request");
				rep = HTTPReply::stock_reply(HTTPReply::forbidden);
			}
			catch(Request::NotFoundException& e)
			{
				Log::GetInstance().debug("Path not found");
				rep = HTTPReply::stock_reply(HTTPReply::not_found);
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
			catch(Exception& e)
			{
				Log::GetInstance().debug("Exception", e);
				rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
			}
			catch(thread_interrupted)
			{
				Log::GetInstance().debug("Current thread externally interrupted");
				rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
				throw thread_interrupted();
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

			SetCurrentThreadWaiting();
		}



		bool ServerModule::URLDecode(
			const std::string& in,
			std::string& out
		){
		  out.clear();
		  out.reserve(in.size());
		  for (std::size_t i = 0; i < in.size(); ++i)
		  {
			if (in[i] == '%')
			{
				if(i+2 <= in.size() && in[i+1] == 'u')
				{
					// Non standard unicode character is rejected
					i+=5;
				}
				else if (i + 3 <= in.size())
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



		const ServerModule::Threads& ServerModule::GetThreads()
		{
			return _threads;
		}



		void ServerModule::KillThread(const string& key, bool autoRestart)
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			Threads::iterator it(_threads.find(key));
			if(it == _threads.end()) return;
			shared_ptr<thread> theThread(it->second.theThread);
			_threads.erase(it);
			theThread->interrupt();
			Log::GetInstance ().info ("Attempted to kill the thread "+ key);
			if(	autoRestart &&
				GetThreadInfo(key).isHTTPThread &&
				_threads.size() < lexical_cast<size_t>(GetParameter(ServerModule::MODULE_PARAM_NB_THREADS))
			){
				thread::id newId(AddHTTPThread());
				Log::GetInstance ().info ("Create the thread "+ lexical_cast<string>(newId) +" because the minimum threads number was reached");
			}
		}



		void ServerModule::KillAllThreads()
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			vector<string> threadsId;
			BOOST_FOREACH(const ServerModule::Threads::value_type it, _threads)
			{
				threadsId.push_back(it.first);
			}
			BOOST_FOREACH(const string& threadId, threadsId)
			{
				ServerModule::KillThread(threadId, false);
			}

			Log::GetInstance ().info ("All threads are killed and HTTP Server is now stopped.");
		}



		void ServerModule::KillAllHTTPThreads(bool autoRestart)
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			vector<string> threadsId;
			BOOST_FOREACH(const ServerModule::Threads::value_type it, _threads)
			{
				if(it.second.isHTTPThread) threadsId.push_back(it.first);
			}
			BOOST_FOREACH(const string& threadId, threadsId)
			{
				ServerModule::KillThread(threadId, false);
			}
			
			if(autoRestart)
			{
				size_t threadsNumber(lexical_cast<size_t>(GetParameter(ServerModule::MODULE_PARAM_NB_THREADS)));
				for (std::size_t i = 0; i < threadsNumber; ++i)
				{
					ServerModule::AddHTTPThread();
				}
			}
			else
			{
				Log::GetInstance ().info ("HTTP Server is now stopped.");
			}
		}


		void ServerModule::Wait()
		{
			while(true)
			{
				shared_ptr<thread> theThread;
				{
					recursive_mutex::scoped_lock lock(_threadManagementMutex);
					if(_threads.empty()) break;
					theThread = _threads.begin()->second.theThread;
				}
				theThread->join();
			}
		}



		void ServerModule::AddThread(
			shared_ptr<thread> theThread,
			const std::string& description,
			bool isHTTPThread
		){
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			ThreadInfo info;
			info.status = ThreadInfo::THREAD_WAITING;
			info.theThread = theThread;
			info.lastChangeTime = posix_time::microsec_clock::local_time();
			info.description = description;
			info.isHTTPThread = isHTTPThread;
			_threads.insert(make_pair(lexical_cast<string>(theThread->get_id()), info));
		}



		boost::thread::id ServerModule::AddHTTPThread()
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			shared_ptr<thread> theThread(
				new thread(
					bind(&asio::io_service::run, &ServerModule::_io_service)
			)	);
			AddThread(theThread, "HTTP", true);
			++_waitingThreads;
			return theThread->get_id();
		}



		void ServerModule::SetCurrentThreadAnalysing( const std::string& queryString )
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetThreadInfo());
				if(info.status != ThreadInfo::THREAD_WAITING) return;
				info.status = ThreadInfo::THREAD_ANALYSING_REQUEST;
				info.queryString = queryString;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
				--_waitingThreads;
				if(_waitingThreads == 0)
				{
					AddHTTPThread();
					Log::GetInstance ().info ("Raised HTTP threads number to "+ lexical_cast<string>(_threads.size()) +" due to pool saturation.");
				}
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		ServerModule::ThreadInfo& ServerModule::GetThreadInfo(
			optional<string> key
		){
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			Threads::iterator it(_threads.find(key ? *key : lexical_cast<string>(this_thread::get_id())));
			if(it == _threads.end()) throw ThreadInfo::Exception();
			return it->second;
		}



		void ServerModule::SetCurrentThreadRunningAction()
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetThreadInfo());
				info.status = ThreadInfo::THREAD_RUNNING_ACTION;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		void ServerModule::SetCurrentThreadRunningFunction()
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetThreadInfo());
				info.status = ThreadInfo::THREAD_RUNNING_FUNCTION;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		void ServerModule::SetCurrentThreadWaiting()
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetThreadInfo());
				if(info.status == ThreadInfo::THREAD_WAITING) return;
				info.status = ThreadInfo::THREAD_WAITING;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
				++_waitingThreads;
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		boost::recursive_mutex& ServerModule::GetThreadManagementMutex()
		{
			return _threadManagementMutex;
		}



		util::EMail ServerModule::GetEMailSender()
		{
			return EMail(
				GetParameter(MODULE_PARAM_SMTP_SERVER),
				GetParameter(MODULE_PARAM_SMTP_PORT)
			);
		}



		std::string ServerModule::URLEncode( const std::string& value )
		{
			stringstream result;
			BOOST_FOREACH(unsigned char c, value)
			{
				if(	(c >= 48 && c <= 57) ||
					(c >= 65 && c <= 90) ||
					(c >= 97 && c <= 122)
				){
					result << c;
				}
				else
				{
					result << "%";
					if(c < 16) result << "0";
					result << hex << static_cast<int>(c);
				}
			}
			return result.str();
		}



		boost::posix_time::time_duration ServerModule::GetSessionMaxDuration()
		{
			return _sessionMaxDuration;
		}



		const char* ServerModule::ThreadInfo::Exception::what() const throw()
		{
			return "Current thread is unregistered. Cannot retrieve thread info.";
		}



		void ServerModule::_SetCookieHeaders(HTTPReply& httpReply, const Request::CookiesMap& cookiesMap)
		{
			BOOST_FOREACH(const Request::CookiesMap::value_type &cookie, cookiesMap)
			{
				// TODO: proper escaping of cookie values
				httpReply.headers.insert(
					make_pair(
						"Set-Cookie",
						cookie.first + "=" + cookie.second.first +
						"; Max-Age=" + lexical_cast<string>(cookie.second.second) + ";"
					)
				);
			}
		}
	}
}

