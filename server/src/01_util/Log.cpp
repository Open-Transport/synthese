
/** Log class implementation.
	@file Log.cpp

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

#include "Log.h"

#include <iomanip>
#include <string.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

#ifdef __gnu_linux__
#include <pthread.h>  // For pthread_getname_np
#include <features.h> // For __GNU_LIBRARY__, __GLIBC__ and __GLIBC_MINOR__
#endif


namespace synthese
{
	namespace util
	{
		std::map<std::string, Log*> Log::_logs;
		Log Log::_defaultLog;

		const std::string Log::DEFAULT_LOG_NAME ("");

		const std::string Log::LOG_PREFIX_TRACE ("TRACE");
		const std::string Log::LOG_PREFIX_DEBUG ("DEBUG");
		const std::string Log::LOG_PREFIX_INFO  ("INFO ");
		const std::string Log::LOG_PREFIX_WARN  ("WARN ");
		const std::string Log::LOG_PREFIX_ERROR ("ERROR");
		const std::string Log::LOG_PREFIX_FATAL ("FATAL");
		const std::string Log::LOG_PREFIX_NONE  ("NONE");



		Log::Log(
			std::ostream* outputStream, Log::Level level
		):	_outputStream (outputStream),
			_level (level)
		{
			if(getenv("SYNTHESE_LOG_LEVEL"))
			{
				int level = lexical_cast<int>(getenv("SYNTHESE_LOG_LEVEL"));
				setLevel(Log::Level(level));
			}
		}



		Log::~Log ()
		{
		}



		Log& Log::GetInstance (const std::string& logName)
		{
			if (logName.empty ()) return _defaultLog;
			if (_logs.find (logName) == _logs.end ())
			{
				_logs.insert (std::make_pair (logName, new Log ()));
			}
			return *(_logs.find (logName)->second);
		}



		void Log::setOutputStream (std::ostream* outputStream)
		{
			_outputStream = outputStream;
		}



		Log::Level Log::getLevel () const
		{
			return _level;
		}



		void Log::setLevel (Log::Level level)
		{
			_level = level;
		}



		void Log::debug (const std::string& message)
		{
			if (_level > Log::LEVEL_DEBUG) return;
			append (Log::LEVEL_DEBUG, message);
		}



		void
		Log::debug (const std::string& message, const std::exception& exception)
		{
			if (_level > Log::LEVEL_DEBUG) return;
			append (Log::LEVEL_DEBUG, message, &exception);
		}



		void
		Log::info (const std::string& message)
		{
			if (_level > Log::LEVEL_INFO) return;
			append (Log::LEVEL_INFO, message);
		}



		void
		Log::info (const std::string& message, const std::exception& exception)
		{
			if (_level > Log::LEVEL_INFO) return;
			append (Log::LEVEL_INFO, message, &exception);
		}



		void
		Log::warn (const std::string& message)
		{
			if (_level > Log::LEVEL_WARN) return;
			append (Log::LEVEL_WARN, message);
		}



		void
		Log::warn (const std::string& message, const std::exception& exception)
		{
			if (_level > Log::LEVEL_WARN) return;
			append (Log::LEVEL_WARN, message, &exception);
		}



		void
		Log::error (const std::string& message)
		{
			if (_level > Log::LEVEL_ERROR) return;
			append (Log::LEVEL_ERROR, message);
		}



		void Log::error (const std::string& message, const std::exception& exception)
		{
			if (_level > Log::LEVEL_ERROR) return;
			append (Log::LEVEL_ERROR, message, &exception);
		}



		void Log::fatal (const std::string& message)
		{
			if (_level > Log::LEVEL_FATAL) return;
			append (Log::LEVEL_FATAL, message);
		}



		void Log::fatal (const std::string& message, const std::exception& exception)
		{
			if (_level > Log::LEVEL_FATAL) return;
			append (Log::LEVEL_FATAL, message, &exception);
		}




		void Log::append(
			Log::Level level,
			const std::string& message,
			const std::exception* exception
		){
			// Standard io streams are not thread safe.
			// Acquire lock here.

			boost::mutex::scoped_lock lock (_ioMutex);

			switch (level)
			{
			case LEVEL_TRACE:
				(*_outputStream) << LOG_PREFIX_TRACE;
				break;

			case Log::LEVEL_DEBUG:
				(*_outputStream) << LOG_PREFIX_DEBUG;
				break;

			case Log::LEVEL_INFO:
				(*_outputStream) << LOG_PREFIX_INFO;
				break;

			case Log::LEVEL_WARN:
				(*_outputStream) << LOG_PREFIX_WARN;
				break;

			case Log::LEVEL_ERROR:
				(*_outputStream) << LOG_PREFIX_ERROR;
				break;

			case Log::LEVEL_FATAL:
				(*_outputStream) << LOG_PREFIX_FATAL;
				break;

			case Log::LEVEL_NONE:
				(*_outputStream) << LOG_PREFIX_NONE;
				break;
			}

			// Append date time
			ptime now(microsec_clock::local_time());

			// Linux only : print thread name instead of thread id
			char threadName[16];
			memset(threadName, 0, sizeof(threadName));
#if defined __gnu_linux__ && __GNU_LIBRARY__ >= 6 && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 13
			pthread_getname_np(pthread_self(), threadName, sizeof(threadName));
#endif

			(*_outputStream) << " # " << std::setfill ('0')
				<< std::setw (4) << now.date().year() << "/"
				<< std::setw (2) << static_cast<int>(now.date().month()) << "/"
				<< std::setw (2) << now.date().day() << " "
				<< std::setw (2) << now.time_of_day().hours() << ":"
				<< std::setw (2) << now.time_of_day().minutes() << ":"
				<< std::setw (2) << now.time_of_day().seconds() << "."
				<< std::setw (3) << now.time_of_day().fractional_seconds();

			if(0 < strlen(threadName))
			{
				(*_outputStream) << setw (6) << " # " << std::string(threadName);
			}
			else
			{
				(*_outputStream) << setw (6) << " # " << this_thread::get_id();
			}

			(*_outputStream) << " # " << message;

			if (exception != 0)
			{
				(*_outputStream) << " : " << exception->what ();
			}

			(*_outputStream) << std::endl;
			// Locks is automatically released when goes out of scope.
		}



		void Log::trace( const std::string& message )
		{
			if (_level > Log::LEVEL_TRACE)
				return;
			append(Log::LEVEL_TRACE, message);
		}
}	}
