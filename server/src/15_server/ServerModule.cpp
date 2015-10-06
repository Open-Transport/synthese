
/** ServerModule class implementation.
    @file ServerModule.cpp

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


#include "ServerModule.h"
#include "EMail.h"
#include "Log.h"

#include <sys/resource.h> // for getrusage
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <fstream>

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
using namespace boost::algorithm;
using namespace boost::iostreams;
using namespace boost::posix_time;
using namespace boost::filesystem;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const std::string util::FactorableTemplate<ModuleClass, ServerModule>::FACTORY_KEY("15_server");

	namespace server
	{
		boost::asio::io_service ServerModule::_io_service;
		boost::asio::ip::tcp::acceptor ServerModule::_acceptor(ServerModule::_io_service);
		connection_ptr ServerModule::_new_connection(new HTTPConnection(ServerModule::_io_service, &ServerModule::HandleRequest));
		ServerModule::Threads ServerModule::_threads;
		recursive_mutex ServerModule::_threadManagementMutex;
		time_duration ServerModule::_sessionMaxDuration(minutes(30));
		string ServerModule::_autoLoginUser("");
		boost::posix_time::ptime ServerModule::_serverStartingTime(not_a_date_time);
		optional<path> ServerModule::_httpTracePath;
		bool ServerModule::_forceGZip(false);

		const string ServerModule::MODULE_PARAM_PORT ("port");
		const string ServerModule::MODULE_PARAM_NB_THREADS ("nb_threads");
		const string ServerModule::MODULE_PARAM_LOG_LEVEL ("log_level");
		const string ServerModule::MODULE_PARAM_SMTP_SERVER ("smtp_server");
		const string ServerModule::MODULE_PARAM_SMTP_PORT ("smtp_port");
		const string ServerModule::MODULE_PARAM_SESSION_MAX_DURATION("session_max_duration");
		const string ServerModule::MODULE_PARAM_AUTO_LOGIN_USER("auto_login_user");
		const string ServerModule::MODULE_PARAM_HTTP_TRACE_PATH = "http_trace_path";
		const string ServerModule::MODULE_PARAM_HTTP_FORCE_GZIP = "http_force_gzip";

		std::string ServerModule::VERSION;
		std::string ServerModule::REVISION;
		std::string ServerModule::BRANCH;
		std::string ServerModule::BUILD_DATE;
		std::string ServerModule::SYNTHESE_URL;

		template<> const string ModuleClassTemplate<ServerModule>::NAME("Server kernel");

		boost::shared_mutex ServerModule::baseWriterMutex;
		boost::shared_mutex ServerModule::InterSYNTHESEAgainstRequestsMutex;
		boost::shared_mutex ServerModule::IneoBDSIAgainstVDVMutex;

		template<> void ModuleClassTemplate<ServerModule>::PreInit()
		{
			RegisterParameter(ServerModule::MODULE_PARAM_PORT, "8080", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_NB_THREADS, "5", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_LOG_LEVEL, "1", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_SMTP_SERVER, "smtp", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_SMTP_PORT, "mail", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_SESSION_MAX_DURATION, "30", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_AUTO_LOGIN_USER, "", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_HTTP_TRACE_PATH, "", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_HTTP_FORCE_GZIP, "", &ServerModule::ParameterCallback);
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
				ServerModule::UpdateStartingTime();
				ServerModule::_acceptor.async_accept(
					ServerModule::_new_connection->socket(),
					boost::bind(&ServerModule::HandleAccept, asio::placeholders::error)
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

		template<> void ModuleClassTemplate<ServerModule>::Start()
		{
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
			UnregisterParameter(ServerModule::MODULE_PARAM_HTTP_TRACE_PATH);

			ServerModule::_io_service.stop();
		}





		template<> void ModuleClassTemplate<ServerModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<ServerModule>::CloseThread(
		){
		}


		void ServerModule::InitRevisionInfo(
				const std::string &version,
				const std::string &revision,
				const std::string &branch,
				const std::string &buildDate,
				const std::string &gitURL
		) {
			VERSION = version;
			REVISION = revision;
			BRANCH = branch;
			BUILD_DATE = buildDate;
			SYNTHESE_URL = gitURL;
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
			if(name == MODULE_PARAM_AUTO_LOGIN_USER)
			{
				_autoLoginUser = value;
			}
			if(name == MODULE_PARAM_HTTP_TRACE_PATH)
			{
				if(value.empty())
				{
					_httpTracePath.reset();
				}
				else
				{
					_httpTracePath = value;
				}
			}
			if(name == MODULE_PARAM_HTTP_FORCE_GZIP)
			{
				_forceGZip = (value == "1");
			}
		}


		void ServerModule::HandleAccept(
			const boost::system::error_code& e
		){
			if (!e)
			{
				_new_connection->start();
			}
			_new_connection.reset(new HTTPConnection(_io_service, &ServerModule::HandleRequest));
			_acceptor.async_accept(
				_new_connection->socket(),
				boost::bind(
					&ServerModule::HandleAccept,
					boost::asio::placeholders::error
				)
			);
		}


		void ServerModule::HandleRequest(
			const HTTPRequest& req,
			HTTPReply& rep
		){

			ptime now(microsec_clock::local_time());
			struct rusage usageStart;
			getrusage(RUSAGE_THREAD, &usageStart);
			string requestName("");

			try
			{
				Log::GetInstance ().debug (req.ipaddr +  " # Received request : " +
					req.uri + " (" + lexical_cast<string>(req.uri.size()) + " bytes)" +
					(req.postData.empty() ?
						string() :
						" + "+ lexical_cast<string>(req.postData.size()) +" bytes of POST data : "+	replace_all_copy(req.postData.substr(0, 1000), "\r\n", string())
					)
				);

				SetCurrentThreadAnalysing(req.uri + (req.postData.empty() ? string() : " + "+ req.postData.substr(0, 100)));
				DynamicRequest request(req);

				auto_ptr<ofstream> of;
				if(_httpTracePath)
				{
					stringstream dateDirName;
					dateDirName <<
						now.date().year() << "-" <<
						setw(2) << setfill('0') << int(now.date().month()) << "-" <<
						setw(2) << setfill('0') << now.date().day()
						;
					stringstream fileName;
					fileName <<
						setw(2) << setfill('0') << now.time_of_day().hours() << "-" <<
						setw(2) << setfill('0') << now.time_of_day().minutes() << "-" <<
						setw(2) << setfill('0') << now.time_of_day().seconds() << "-" << now.time_of_day().fractional_seconds() <<
						"_";
					if(request.getFunction().get())
					{
						fileName << request.getFunction()->getFactoryKey();
					}
					fileName << ".log";
					path p(*_httpTracePath);
					p = p / dateDirName.str();
					create_directories(p);
					p = p / fileName.str();
					of.reset(new ofstream(p.string().c_str()));
					*of << "GET " << req.uri << "\n";
					*of << "POST\n" << req.postData << "\n";
				}

				// GZip compression ?
				bool gzipCompression(false);
				HTTPRequest::Headers::const_iterator it(req.headers.find("Accept-Encoding"));
				if(	it != req.headers.end() &&
					!it->second.empty()
				){
					set<string> formats;
					split(formats, it->second, is_any_of(","));
					gzipCompression = (formats.find("gzip") != formats.end());
				}

				// Request run
				stringstream ros;
				{
					// Don't request if interSYNTHESE is writing
					boost::shared_lock<boost::shared_mutex> lock(ServerModule::InterSYNTHESEAgainstRequestsMutex);
					request.run(ros);
				}
				// Now the request name has been evaluated
				requestName = request.name;

				// Output
				if(	_forceGZip ||
					(	gzipCompression &&
						req.ipaddr != "127.0.0.1" // Never compress for localhost use
				)	){
					std::string rosContent = ros.str();

					// Do not compress an empty response because it breaks web browsers
					if(0 < rosContent.size())
					{
						stringstream os;
						filtering_stream<output> fs;
						fs.push(gzip_compressor());
						fs.push(os);
						boost::iostreams::copy(ros, fs);
						fs.pop();
						rep.content.append(os.str());
						rep.headers.insert(make_pair("Content-Encoding", "gzip"));
					}
				}
				else
				{
					rep.content.append(ros.str());
				}
				rep.status = HTTPReply::ok;
				rep.headers.insert(make_pair("Content-Length", lexical_cast<string>(rep.content.size())));
				rep.headers.insert(make_pair("Content-Type", request.getOutputMimeType() + "; charset=utf-8"));
				if(request.getFunction().get() && !request.getFunction()->getFileName().empty())
				{
					rep.headers.insert(make_pair("Content-Disposition", "attachement; filename="+ request.getFunction()->getFileName()));
				}
				if(request.getFunction().get() && !request.getFunction()->getMaxAge().is_not_a_date_time())
				{
					rep.headers.insert(make_pair("Cache-Control", "public, max-age="+
												 lexical_cast<string>(request.getFunction()->getMaxAge().total_seconds())));
				}
				else
				{
					_SetCookieHeaders(rep, request.getCookiesMap());
				}

				if(_httpTracePath)
				{
					*of << "\n\nRESPONSE\n\n";
					*of << ros.str();
					of.reset();
				}

			}
			catch(Request::RedirectException& e)
			{
				rep = HTTPReply::stock_reply(
					e.getPermanently() ? HTTPReply::moved_permanently : HTTPReply::moved_temporarily
				);
				rep.headers.insert(make_pair("Location", e.getLocation()));
				_SetCookieHeaders(rep, e.getCookiesMap());
			}
			catch(Request::ForbiddenRequestException&)
			{
				Log::GetInstance().debug("Forbidden request");
				rep = HTTPReply::stock_reply(HTTPReply::forbidden);
			}
			catch(Request::NotFoundException&)
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

			struct rusage usageEnd;
			getrusage(RUSAGE_THREAD, &usageEnd);
			time_duration cpuUsage( (seconds(usageEnd.ru_utime.tv_sec) + microsec(usageEnd.ru_utime.tv_usec)) -
									(seconds(usageStart.ru_utime.tv_sec) + microsec(usageStart.ru_utime.tv_usec)) );

			time_duration td = microsec_clock::local_time() - now;
			Log::GetInstance ().debug (req.ipaddr +
									   " # request duration (ms): " + lexical_cast<string>(td.total_milliseconds())
									   + " cpu: " + lexical_cast<string>(cpuUsage.total_milliseconds())
									   + " status: " + lexical_cast<string>(rep.status)
									   + " request: " + requestName);

			SetCurrentThreadWaiting();
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
			boost::shared_ptr<thread> theThread(it->second.theThread);

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
				boost::shared_ptr<thread> theThread;
				{
					recursive_mutex::scoped_lock lock(_threadManagementMutex);
					if(_threads.empty()) break;
					theThread = _threads.begin()->second.theThread;
				}
				theThread->join();
			}
		}



		boost::thread::id ServerModule::AddHTTPThread()
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			static unsigned int httpThreadCounter = 0;
			httpThreadCounter += 1;

			boost::shared_ptr<thread> theThread(
				AddThread(
					boost::bind(&asio::io_service::run, &ServerModule::_io_service),
					"HTTP_" + boost::lexical_cast<std::string>(httpThreadCounter),
					true
			)	);
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
			}
			catch (ThreadInfo::Exception&)
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
				Log::GetInstance().trace("Thread "+ info.description +"("+ lexical_cast<string>(info.theThread)  +") is now running the action "+ info.queryString);
				info.status = ThreadInfo::THREAD_RUNNING_ACTION;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
			}
			catch (ThreadInfo::Exception&)
			{
			}
		}



		void ServerModule::SetCurrentThreadRunningFunction()
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetThreadInfo());
				Log::GetInstance().trace("Thread "+ info.description +"("+ lexical_cast<string>(info.theThread)  +") is now running the service "+ info.queryString);
				info.status = ThreadInfo::THREAD_RUNNING_FUNCTION;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
			}
			catch (ThreadInfo::Exception&)
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
				Log::GetInstance().trace("Thread "+ info.description +"("+ lexical_cast<string>(info.theThread)  +") is now waiting");
				info.status = ThreadInfo::THREAD_WAITING;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
			}
			catch (ThreadInfo::Exception&)
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



		boost::posix_time::time_duration ServerModule::GetSessionMaxDuration()
		{
			return _sessionMaxDuration;
		}



		const char* ServerModule::ThreadInfo::Exception::what() const throw()
		{
			return "Current thread is unregistered. Cannot retrieve thread info.";
		}



		void ServerModule::_SetCookieHeaders(HTTPReply& httpReply, const CookiesMap& cookiesMap)
		{
			BOOST_FOREACH(const CookiesMap::value_type &cookie, cookiesMap)
			{
				// TODO: proper escaping of cookie values
				httpReply.headers.insert(
					make_pair(
						"Set-Cookie",
						cookie.first + "=" + cookie.second.first +
						"; Max-Age=" + lexical_cast<string>(cookie.second.second) + "; Path=/; "
					)
				);
			}
		}



		void ServerModule::UpdateStartingTime()
		{
			_serverStartingTime = second_clock::local_time();
		}



		const boost::posix_time::ptime& ServerModule::GetStartingTime()
		{
			return _serverStartingTime;
		}

}	}
