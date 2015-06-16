
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

#include <boost/filesystem/path.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

#undef GetObject // due to WinGDI.h
#undef VERSION // due to mysql.h

#include "CallableByThread.hpp"
#include "ModuleClassTemplate.hpp"
#include "ServerTypes.h"
#ifdef __gnu_linux__
#include <pthread.h> // For pthread_setname_np
#include <features.h>
#endif
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
				boost::posix_time::ptime lastChangeTime;
			};

			typedef std::map<std::string, ThreadInfo> Threads;

			//! DbModule parameters
			static const std::string MODULE_PARAM_PORT;
			static const std::string MODULE_PARAM_LOG_LEVEL;
			static const std::string MODULE_PARAM_SMTP_SERVER;
			static const std::string MODULE_PARAM_SMTP_PORT;
			static const std::string MODULE_PARAM_SESSION_MAX_DURATION;
			static const std::string MODULE_PARAM_AUTO_LOGIN_USER;
			static const std::string MODULE_PARAM_HTTP_TRACE_PATH;
			static const std::string MODULE_PARAM_HTTP_FORCE_GZIP;

			static std::string VERSION;
			static std::string REVISION;
			static std::string BRANCH;
			static std::string BUILD_DATE;
			static std::string SYNTHESE_URL;

			// SYNTHESE is not lock protected against changing the base content
			// while reading or writing it. Take this mutex if you change the base
			// in a service.
			// @FIXME This should be used by all services appropriately.
			static boost::shared_mutex baseWriterMutex;
			static boost::shared_mutex InterSYNTHESEAgainstRequestsMutex;
			static boost::shared_mutex IneoBDSIAgainstVDVMutex;

		private:

			// Threads
			static Threads _threads;
			static boost::recursive_mutex _threadManagementMutex;
			static boost::posix_time::time_duration _sessionMaxDuration;
			static std::string _autoLoginUser;
			static boost::posix_time::ptime _serverStartingTime;
			static boost::optional<boost::filesystem::path> _httpTracePath;
			static bool _forceGZip;

		public:
			static void InitRevisionInfo(const std::string &version,
				const std::string &revision,
				const std::string &branch,
				const std::string &buildDate,
				const std::string &gitURL
			);

			template<class Callable>
			static boost::shared_ptr<boost::thread> AddThread(
				Callable func,
				const std::string& description
			);
			static void KillThread(const std::string& key);
			static void KillAllThreads();
			static void KillAllHTTPThreads();
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

			/** Called whenever a parameter registered by this module is changed
			 */
			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);


		};



		template<class Callable>
		boost::shared_ptr<boost::thread> ServerModule::AddThread(
			Callable func,
			const std::string& description
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
			_threads.insert(
				std::make_pair(boost::lexical_cast<std::string>(theThread->get_id()), info)
			);
#if defined __gnu_linux__ && __GNU_LIBRARY__ >= 6 && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 13
			pthread_setname_np(theThread->native_handle(), description.substr(0, 15).c_str());
#endif
			return theThread;
		}
	}
	/** @} */
}

#endif // SYNTHESE_ServerModule_H__
