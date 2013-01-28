////////////////////////////////////////////////////////////////////////////////
/// Log class header.
///	@file Log.h
///	@author Marc Jambert
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

#ifndef SYNTHESE_UTIL_LOG_H
#define SYNTHESE_UTIL_LOG_H

#include <map>
#include <string>
#include <iostream>
#include <boost/thread/mutex.hpp>

namespace synthese
{
	namespace util
	{
		//lint --e{1704}

		////////////////////////////////////////////////////////////////////
		/// Thread-safe logging class.
		///	To get an instance of this class, the statis GetLog method must
		///	be invoked. With no argument, the default log is returned.
		///
		///	Be careful : this class is not designed for efficiency,
		///	so log carefully...
		///
		///	@ingroup m01
		class Log
		{
		 public:

			typedef enum
			{
				LEVEL_TRACE = -1
 				, LEVEL_DEBUG = 0,
				   LEVEL_INFO,
				   LEVEL_WARN,
				   LEVEL_ERROR,
				   LEVEL_FATAL,
				   LEVEL_NONE
			} Level;

		 private:

			static const std::string DEFAULT_LOG_NAME;
			static std::map<std::string, Log*> _logs;
			static Log _defaultLog;

			boost::mutex _ioMutex; //!< For thread safety.

			std::ostream* _outputStream; //!< Log output stream.
			Log::Level _level; //!< Log level.

			Log(
				std::ostream* outputStream = &std::cout,
				Log::Level level = Log::LEVEL_DEBUG
			);

		 public:

			~Log ();

			static Log& GetInstance (const std::string& logName = DEFAULT_LOG_NAME);

			//! @name Getters/Setters
			//@{
				void setOutputStream (std::ostream* outputStream);

				Log::Level getLevel () const;
				void setLevel (Log::Level level);
			//@}

			//! @name Update methods
			//@{
				void trace(const std::string& message);

				void debug (const std::string& message);
				void debug (const std::string& message, const std::exception& exception);

				void info (const std::string& message);
				void info (const std::string& message, const std::exception& exception);

				void warn (const std::string& message);
				void warn (const std::string& message, const std::exception& exception);

				void error (const std::string& message);
				void error (const std::string& message, const std::exception& exception);

				void fatal (const std::string& message);
				void fatal (const std::string& message, const std::exception& exception);
			//@}

		private:
			static const std::string LOG_PREFIX_TRACE;
			static const std::string LOG_PREFIX_DEBUG;
			static const std::string LOG_PREFIX_INFO;
			static const std::string LOG_PREFIX_WARN;
			static const std::string LOG_PREFIX_ERROR;
			static const std::string LOG_PREFIX_FATAL;
			static const std::string LOG_PREFIX_NONE;

			void append(
				Log::Level level,
				const std::string& message,
				const std::exception* exception = 0
			);
		};
}	}

#endif
