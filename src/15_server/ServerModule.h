
////////////////////////////////////////////////////////////////////////////////
/// Server Module class header.
///	@file ServerModule.h
///	@author Hugues Romain
///
/// @warning This header must always be included before all other ones to avoid
/// the windows bug "WinSock.h has already been included"
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ServerModule_H__
#define SYNTHESE_ServerModule_H__

#include <boost/asio.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#undef GetObject // due to WinGDI.h
#undef VERSION // due to mysql.h

#include "CallableByThread.hpp"
#include "ModuleClassTemplate.hpp"
#include "HTTPConnection.hpp"
#include "ServerTypes.h"

namespace synthese
{
	namespace util
	{
		class EMail;
	}

	/**	@defgroup m15Actions 15 Actions
		@ingroup m15

		@defgroup m15Functions 15 Functions
		@ingroup m15

		@defgroup m15Pages 15 Pages
		@ingroup m15

		@defgroup m15Library 15 Interface library
		@ingroup m15

		@defgroup m15LS 15 Table synchronizers
		@ingroup m15

		@defgroup m15Admin 15 Administration pages
		@ingroup m15

		@defgroup m15Rights 15 Rights
		@ingroup m15

		@defgroup m15Logs 15 DB Logs
		@ingroup m15

		@defgroup m15 15 Server
		@ingroup m1
		@{
	*/

	/** 15 Server module namespace.
	*/
	namespace server
	{
		class Session;
		struct HTTPRequest;
		struct HTTPReply;

		/** 15 Server module class.
		*/
		class ServerModule:
			public ModuleClassTemplate<ServerModule>
		{
			friend class ModuleClassTemplate<ServerModule>;

		public:
			struct ThreadInfo
			{
				typedef enum
				{
					THREAD_WAITING,
					THREAD_ANALYSING_REQUEST,
					THREAD_RUNNING_ACTION,
					THREAD_RUNNING_FUNCTION
				} Status;

				class Exception : public std::exception
				{
				public:
					const char* what() const throw();
				};

				boost::shared_ptr<boost::thread> theThread;
				Status status;
				std::string queryString;
				std::string description;
				bool isHTTPThread;
				boost::posix_time::ptime lastChangeTime;
			};

			typedef std::map<std::string, ThreadInfo> Threads;

			//! DbModule parameters
			static const std::string MODULE_PARAM_PORT;
			static const std::string MODULE_PARAM_NB_THREADS;
			static const std::string MODULE_PARAM_LOG_LEVEL;
			static const std::string MODULE_PARAM_SMTP_SERVER;
			static const std::string MODULE_PARAM_SMTP_PORT;
			static const std::string MODULE_PARAM_SESSION_MAX_DURATION;
			static const std::string MODULE_PARAM_AUTO_LOGIN_USER;
			static const std::string MODULE_PARAM_HTTP_TRACE_PATH;
			static const std::string MODULE_PARAM_HTTP_FORCE_GZIP;

			static const std::string VERSION;
			static const std::string REVISION;
			static const std::string BUILD_DATE;
			static const std::string SYNTHESE_URL;

			// SYNTHESE is not lock protected against changing the base content
			// while reading or writing it. Take this mutex if you change the base
			// in a service.
			// @FIXME This should be used by all services appropriately.
			static boost::shared_mutex baseWriterMutex;
			static boost::shared_mutex InterSYNTHESEAgainstRequestsMutex;

		private:

			/// The io_service used to perform asynchronous operations.
			static boost::asio::io_service _io_service;

			/// Acceptor used to listen for incoming connections.
			static boost::asio::ip::tcp::acceptor _acceptor;

			/// The next connection to be accepted.
			static connection_ptr _new_connection;

			// Threads
			static Threads _threads;
			static boost::recursive_mutex _threadManagementMutex;
			static boost::posix_time::time_duration _sessionMaxDuration;
			static std::string _autoLoginUser;
			static boost::posix_time::ptime _serverStartingTime;
			static boost::optional<boost::filesystem::path> _httpTracePath;
			static bool _forceGZip;

		public:
			static boost::thread::id AddHTTPThread();
			
			template<class Callable>
			static boost::shared_ptr<boost::thread> AddThread(
				Callable func,
				const std::string& description,
				bool isHTTPThread = false
			);
			static void KillThread(const std::string& key, bool autoRestart=true);
			static void KillAllThreads();
			static void KillAllHTTPThreads(bool autoRestart=true);
			static void Wait();
			static const Threads& GetThreads();
			static void SetCurrentThreadAnalysing(const std::string& queryString);
			static void SetCurrentThreadRunningAction();
			static void SetCurrentThreadRunningFunction();
			static void SetCurrentThreadWaiting();
			static ThreadInfo& GetThreadInfo(boost::optional<std::string> key = boost::optional<std::string>());
			static boost::recursive_mutex& GetThreadManagementMutex();
			static void RunHTTPServer();
			static util::EMail GetEMailSender();
			static const std::string& GetAutoLoginUser() { return _autoLoginUser; }
			static void UpdateStartingTime();
			static const boost::posix_time::ptime& GetStartingTime();

			static boost::posix_time::time_duration GetSessionMaxDuration();

			static const std::string GetBranch();

			/** Called whenever a parameter registered by this module is changed
			 */
			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);


		public:
			/// Handle completion of an asynchronous accept operation.
			static void HandleAccept(
				const boost::system::error_code& e
			);

			/// Handle a request and produce a reply.
			/// @param req HTTP request to handle
			/// @param rep HTTP Reply to write the result on
			static void HandleRequest(
				const HTTPRequest& req,
				HTTPReply& rep
			);

		private:
			/// Sets headers in the given HTTPReply from the cookies stored in cookiesMap.
			static void _SetCookieHeaders(
				HTTPReply& httpReply,
				const CookiesMap& cookiesMap
			);
		};



		template<class Callable>
		boost::shared_ptr<boost::thread> ServerModule::AddThread(
			Callable func,
			const std::string& description,
			bool isHTTPThread /*= false */
		){

			boost::recursive_mutex::scoped_lock lock(_threadManagementMutex);

			CallableByThread<Callable> bnd(func);

			boost::shared_ptr<boost::thread> theThread(
				new boost::thread(bnd)
			);

			ThreadInfo info;
			info.status = ThreadInfo::THREAD_WAITING;
			info.theThread = theThread;
			info.lastChangeTime = boost::posix_time::microsec_clock::local_time();
			info.description = description;
			info.isHTTPThread = isHTTPThread;
			_threads.insert(
				std::make_pair(boost::lexical_cast<std::string>(theThread->get_id()), info)
			);

			return theThread;
		}
	}
	/** @} */
}

#endif // SYNTHESE_ServerModule_H__
