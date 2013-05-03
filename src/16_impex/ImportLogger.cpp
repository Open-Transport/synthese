
/** ImportLogger class implementation.
	@file ImportLogger.cpp

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

#include "ImportLogger.hpp"

#include <boost/date_time.hpp>
#include <boost/filesystem/convenience.hpp>

using namespace boost::posix_time;
using namespace boost::filesystem;
using namespace std;

namespace synthese
{
	namespace impex
	{
		ImportLogger::ImportLogger(
			Level minLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream
		):	_minLevel(minLevel),
			_outputStream(outputStream)
		{
			if(!logPath.empty())
			{
				ptime now(second_clock::local_time());
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
					setw(2) << setfill('0') << now.time_of_day().seconds() <<
					".log"
				;
				path p(logPath);
				p = p / dateDirName.str();
				create_directories(p);
				p = p / fileName.str();
				_fileStream.reset(new ofstream(p.file_string().c_str()));
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Stores a log entry in the content.
		/// If the output is activated, the entry is sent to the stream immediately
		/// to ensure that the log is readable in case of crash
		/// @param level the level of the entry
		/// @param content the text of the entry
		void ImportLogger::log(
			Level level,
			const std::string& content
		) const {
			if(level >= _minLevel)
			{
				stringstream stream;
				switch(level)
				{
				case DEBG: stream << "DEBG"; break;
				case LOAD: stream << "LOAD"; break;
				case CREA: stream << "CREA"; break;
				case INFO: stream << "INFO"; break;
				case WARN: stream << "WARN"; break;
				case NOTI: stream << "NOTI"; break;
				case ERROR: stream << "ERR "; break;
				case ALL: stream << "ALL "; break;
				case NOLOG: stream << "NOLOG "; break;
				}
				stream << " " << content << "<br />";

				if(_outputStream)
				{
					*_outputStream << stream.str();
				}
				if(_fileStream.get())
				{
					*_fileStream << stream.str();
				}
			}
			if(level > _maxLoggedLevel)
			{
				_maxLoggedLevel = level;
			}
		}



		void ImportLogger::logRaw( const std::string& content ) const
		{
			if(_outputStream)
			{
				*_outputStream << content;
			}
			if(_fileStream.get())
			{
				*_fileStream << content;
			}
		}



		void ImportLogger::logInfo( const std::string& content ) const
		{
			log(INFO, content);
		}



		void ImportLogger::logDebug( const std::string& content ) const
		{
			log(DEBG, content);
		}



		void ImportLogger::logWarning( const std::string& content ) const
		{
			log(WARN, content);
		}



		void ImportLogger::logError( const std::string& content ) const
		{
			log(ERROR, content);
		}



		void ImportLogger::logCreation( const std::string& content ) const
		{
			log(CREA, content);
		}



		void ImportLogger::logLoad( const std::string& content ) const
		{
			log(LOAD, content);
		}
}	}

