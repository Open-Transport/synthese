
/** Importer class implementation.
	@file Importer.cpp

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

#include "Importer.hpp"

#include "DBTransaction.hpp"
#include "Env.h"

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

using namespace boost::filesystem;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;

	namespace impex
	{
		const string Importer::ATTR_IMPORT_START_TIME = "import_start_time";
		const string Importer::TAG_LOG_ENTRY = "log_entry";
		const string Importer::ATTR_LEVEL = "level";
		const string Importer::ATTR_TEXT = "text";



		//////////////////////////////////////////////////////////////////////////
		void Importer::_log(
			ImportLogLevel level,
			const std::string& content
		) const	{
			if(level >= _minLogLevel)
			{
				// String level code
				string levelStr;
				switch(level)
				{
				case IMPORT_LOG_TRAC: levelStr = "TRAC"; break;
				case IMPORT_LOG_DEBG: levelStr = "DEBG"; break;
				case IMPORT_LOG_LOAD: levelStr = "LOAD"; break;
				case IMPORT_LOG_CREA: levelStr = "CREA"; break;
				case IMPORT_LOG_INFO: levelStr = "INFO"; break;
				case IMPORT_LOG_WARN: levelStr = "WARN"; break;
				case IMPORT_LOG_NOTI: levelStr = "NOTI"; break;
				case IMPORT_LOG_ERROR: levelStr = "ERR "; break;
				case IMPORT_LOG_ALL: levelStr = "ALL "; break;
				case IMPORT_LOG_NOLOG: levelStr = "NOLOG "; break;
				}

				// Output stream
				if(_outputStream)
				{
					*_outputStream << levelStr << " " << content << "<br />";
				}

				// File stream
				if(_fileStream.get())
				{
					*_fileStream << levelStr << " " << content << "<br />" << endl;
				}

				// Parameters map
				boost::shared_ptr<ParametersMap> entryPM(new ParametersMap);
				entryPM->insert(ATTR_LEVEL, levelStr);
				entryPM->insert(ATTR_TEXT, content);
				_pm.insert(TAG_LOG_ENTRY, entryPM);
			}
			if(level > _maxLoggedLevel)
			{
				_maxLoggedLevel = level;
			}
		}



		Importer::Importer(
			util::Env& env,
			const Import& import,
			ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	_env(env),
			_import(import),
			_maxLoggedLevel(IMPORT_LOG_TRAC),
			_minLogLevel(minLogLevel),
			_outputStream(outputStream),
			_pm(pm),
			_logPath(logPath)
		{}



		void Importer::openLogFile() const
		{
			ptime now(second_clock::local_time());

			// Parameters map
			stringstream dateStr;
			dateStr <<
				now.date().year() << "-" <<
				setw(2) << setfill('0') << int(now.date().month()) << "-" <<
				setw(2) << setfill('0') << now.date().day() << " " <<
				setw(2) << setfill('0') << now.time_of_day().hours() << ":" <<
				setw(2) << setfill('0') << now.time_of_day().minutes() << ":" <<
				setw(2) << setfill('0') << now.time_of_day().seconds();
			_pm.insert(ATTR_IMPORT_START_TIME, dateStr.str());

			// File stream
			if(!_logPath.empty())
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
					setw(2) << setfill('0') << now.time_of_day().seconds() <<
					".log"
					;
				path p(_logPath);
				p = p / dateDirName.str();
				create_directories(p);
				p = p / fileName.str();
				_fileStream.reset(new ofstream(p.string().c_str()));
			}
		}



		DBTransaction Importer::save() const
		{
			DBTransaction result(_save());
			return result;
		}



		void Importer::_logError( const std::string& content ) const
		{
			_log(IMPORT_LOG_ERROR, content);
		}



		void Importer::_logWarning( const std::string& content ) const
		{
			_log(IMPORT_LOG_WARN, content);
		}



		void Importer::_logDebug( const std::string& content ) const
		{
			_log(IMPORT_LOG_DEBG, content);
		}



		void Importer::_logTrace( const std::string& content ) const
		{
			_log(IMPORT_LOG_TRAC, content);
		}



		void Importer::_logInfo( const std::string& content ) const
		{
			_log(IMPORT_LOG_INFO, content);
		}



		void Importer::_logLoad( const std::string& content ) const
		{
			_log(IMPORT_LOG_LOAD, content);
		}



		void Importer::_logCreation( const std::string& content ) const
		{
			_log(IMPORT_LOG_CREA, content);
		}



		void Importer::_logRaw( const std::string& content ) const
		{
			// Output stream
			if(_outputStream)
			{
				*_outputStream << content;
			}

			// File stream
			if(_fileStream.get())
			{
				*_fileStream << content;
			}

			// Parameters map
			boost::shared_ptr<ParametersMap> entryPM(new ParametersMap);
			entryPM->insert(ATTR_TEXT, content);
			_pm.insert(TAG_LOG_ENTRY, entryPM);
		}



		void Importer::closeLogFile(
			bool result,
			bool simulation,
			const boost::posix_time::ptime& startTime
		) const	{

			{
				stringstream dateStr;
				ptime now(second_clock::local_time());
				dateStr <<
					now.date().year() << "-" <<
					setw(2) << setfill('0') << int(now.date().month()) << "-" <<
					setw(2) << setfill('0') << now.date().day() << " " <<
					setw(2) << setfill('0') << now.time_of_day().hours() << ":" <<
					setw(2) << setfill('0') << now.time_of_day().minutes() << ":" <<
					setw(2) << setfill('0') << now.time_of_day().seconds();

				_logInfo("Import has ended at "+ dateStr.str());
			}

			{
				stringstream dateStr;
				dateStr <<
					startTime.date().year() << "-" <<
					setw(2) << setfill('0') << int(startTime.date().month()) << "-" <<
					setw(2) << setfill('0') << startTime.date().day() << " " <<
					setw(2) << setfill('0') << startTime.time_of_day().hours() << ":" <<
					setw(2) << setfill('0') << startTime.time_of_day().minutes() << ":" <<
					setw(2) << setfill('0') << startTime.time_of_day().seconds();

				_logInfo("Import was started at "+ dateStr.str());
			}

			_logInfo("Import was done in "+ string(simulation ? "simulation" : "real") +" mode.");
			_logInfo("Import has "+ string(result ? "succeeded" : "failed") +".");

			// File stream
			if(_fileStream.get())
			{
				_fileStream.reset();
			}
		}
}	}
