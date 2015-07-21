
/** CalendarOGTFileFormat class implementation.
	@file CalendarOGTFileFormat.cpp

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

#include "PTDataCleanerFileFormat.hpp"
#include "CalendarFileFormat.hpp"
#include "CalendarOGTFileFormat.hpp"
#include "OGTFileFormat.hpp"

#include "Import.hpp"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElementTableSync.h"

#include <exception>
#include <fstream>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace data_exchange;
	using namespace impex;
	using namespace calendar;
	using namespace db;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, CalendarOGTFileFormat>::FACTORY_KEY("CalendarOGT");
	}

	namespace data_exchange
	{
		const std::string CalendarOGTFileFormat::Importer_::SEP(";");



		bool CalendarOGTFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath
		) const {
			_calendarElementsToRemove.clear();
			DataSource& dataSource(*_import.get<DataSource>());

			ifstream inFile;
			inFile.open(filePath.string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.string());
			}
			string line;
			_logDebug(
				"Loading file "+ filePath.string()
			);

			// To use later DataSource& dataSource(*_import.get<DataSource>());

			// Calendar name -> { list of dates }
			CalendarDatesMap calendarDates;

			// Values from line
			std::vector<std::string> values;

			while(getline(inFile, line))
			{
				if(line.empty()) continue;

				values.clear();
				split(values, line, is_any_of(SEP));

				string date(trim_copy(values[0]));
				string ogtFile(trim_copy(values[1]));

				calendarDates[ogtFile].push_back(date);

				_logDebug(
					"Date " + date + " added to plan " + ogtFile + " / #entries: "
					+ boost::lexical_cast<std::string>(calendarDates[ogtFile].size())
				);
			}

			// Parameters to import OGT files
			util::ParametersMap pm(_getParametersMap());

			BOOST_FOREACH(CalendarDatesMap::value_type& aCalendarPair, calendarDates)
			{
				// Calendar name : if the .xml extension exists, remove it
				string calendarName(aCalendarPair.first);

				if (boost::filesystem::extension(calendarName) == ".xml")
				{
					calendarName.erase(calendarName.length() - 4);
				}

				const DayList& daysList(aCalendarPair.second);

				DatesVector datesList;

				BOOST_FOREACH(const string& aDay, daysList)
				{
					try {
						// Parse day string to date
						const date aDate(from_uk_string(aDay));
						datesList.push_back(aDate);
					}
					catch(const std::exception& e)
					{
						_logError("Check date format: '" + aDay + "'. Exception: "
							+ boost::lexical_cast<std::string>(e.what()));
						// Next date by the way
						continue;
					}
				}

				// Existing calendar. Invoke PTDataCleanerFileFormat::beforeParsing for cleanup
				calendar::CalendarTemplate* calPointer = _getCalendarTemplate(_calendarTemplates, calendarName);
				if (calPointer)
				{
					pm.insert(PTDataCleanerFileFormat::PARAMETER_CALENDAR_ID, calPointer->getKey());
					_ogtImporter._setFromParametersMap(pm);
					_logDebug(
						"Cleaning PT data related to calendar "
						+ calendarName + " ID=" + boost::lexical_cast<std::string>(calPointer->getKey())
					);
					_ogtImporter.beforeParsing();
				}

				_logDebug(
					"Processing calendar " + calendarName
				);
				boost::shared_ptr<CalendarTemplate> currentCalendar = _createOrUpdateCalendarTemplate(
						_calendarTemplates,
						calendarName,
						calendarName,
						dataSource,
						datesList);

				if (!currentCalendar) {
					_logError("Failed to create or update calendar '" + calendarName + " with "
						+ boost::lexical_cast<std::string>(datesList.size()) + " dates");
					// Next calendar by the way
					continue;
				}

				// Calendar key replaced
				pm.insert(PTDataCleanerFileFormat::PARAMETER_CALENDAR_ID, currentCalendar->getKey());
				_ogtImporter._setFromParametersMap(pm);

				// Path to calendar's OGT file
				const string& ogtFileName(calendarName + ".xml");
				const boost::filesystem::path& ogtPath =
					filePath.parent_path() / ogtFileName;

				_logDebug(
					"Preparing import of " + ogtPath.string() + " into calendar "
					+ calendarName + " ID=" + boost::lexical_cast<std::string>(currentCalendar->getKey())
				);

				// Run OGT import
				// and propage current Importer log file stream to OGTFileFormat
				bool parseResult = _ogtImporter._parse(ogtPath);

				// The parse method feeds _env registries

				if(!parseResult) {
					_logError("Failed to parse " + ogtPath.string());
				}
				else
				{
					_logInfo("OGT file parsing succeeded: " + ogtPath.string());
				}
			}
			return true;
		}



		db::DBTransaction CalendarOGTFileFormat::Importer_::_save() const
		{
			// Save modified data in OGTFileFormant in transaction
			DBTransaction transaction = _ogtImporter._save();

			// Save calendars
			_saveCalendarTemplates(transaction);

			return transaction;
		}



		CalendarOGTFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			OneFileTypeImporter<CalendarOGTFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			CalendarFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_ogtImporter(env, import, minLogLevel, logPath, outputStream, pm),
			_calendarTemplates(*import.get<DataSource>(), env)
		{}



		util::ParametersMap CalendarOGTFileFormat::Importer_::_getParametersMap() const
		{
			return _ogtImporter._getParametersMap(); // Already contains the PTDataCleanerFileFormat parameters map
		}



		void CalendarOGTFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_ogtImporter._setFromParametersMap(map);
		}



		bool CalendarOGTFileFormat::Importer_::afterParsing()
		{
			return _ogtImporter.afterParsing();
		}

}	}
