
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

				boost::filesystem::path p(calendarName);

				if (p.extension() == ".xml")
				{
					 calendarName.erase(calendarName.length() - 4);
				}

				const DayList& datesList(aCalendarPair.second);

				_logDebug(
					"Processing calendar " + calendarName
				);
				boost::shared_ptr<CalendarTemplate> currentCalendar =
					_createOrUpdateCalendar(calendarName, datesList);

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
				bool parseResult = _ogtImporter._parse(ogtPath, _fileStream);

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



		boost::shared_ptr<CalendarTemplate> CalendarOGTFileFormat::Importer_::_createOrUpdateCalendar(const string& calendarName, DayList datesList) const
		{
			CalendarTemplateTableSync::SearchResult result(
				CalendarTemplateTableSync::Search(
					_env, optional<string>(calendarName), optional<RegistryKeyType>()
			)	);

			boost::shared_ptr<CalendarTemplate> currentCalendar;

			if(!result.empty())
			{
				// Existing calendar. Clear elements
				RegistryKeyType id(result[0].get()->getKey());
				currentCalendar = CalendarTemplateTableSync::GetEditable(id, _env);

				// Iterate on existing calendar elements
				CalendarTemplateElementTableSync::SearchResult oldCalendarElements(
					CalendarTemplateElementTableSync::Search(_env, id)
				);
				BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& oldElement, oldCalendarElements)
				{
					_calendarElementsToRemove.insert(oldElement);
					_env.getEditableRegistry<CalendarTemplateElement>().remove(oldElement->getKey());
					currentCalendar->removeElement(*oldElement);
				}
			}
			else
			{
				// New calendar
				currentCalendar.reset(new CalendarTemplate(CalendarTemplateTableSync::getId()));
				currentCalendar->setName(calendarName);
				_env.getEditableRegistry<CalendarTemplate>().add(currentCalendar);
			}

			size_t rank(0);

			BOOST_FOREACH(const string& aDay, datesList)
			{
				boost::shared_ptr<CalendarTemplateElement>
					element(new CalendarTemplateElement(CalendarTemplateElementTableSync::getId()));

				try {
					// Add an element per single day
					const date aDate(from_uk_string(aDay));

					// try/catch to handle invalid date formats
					element->setMinDate(aDate);
					element->setMaxDate(aDate);
				}
				catch(const std::exception& e)
				{
					_logError("Check date format: '" + aDay + "'. Exception: "
						+ boost::lexical_cast<std::string>(e.what()));
				}

				element->setRank(rank++);
				element->setOperation(CalendarTemplateElement::ADD);
				element->setStep(days(1));
				element->setCalendar(currentCalendar.get());
				currentCalendar->addElement(const_cast<CalendarTemplateElement&>(*element));
				// Insert in registry
				_env.getEditableRegistry<CalendarTemplateElement>().add(element);
			}
			_logInfo(
				"Calendar " + calendarName +
				(result.empty() ? " created with " : " replaced with ")
				+ boost::lexical_cast<std::string>(rank) + " dates"
			);

			return currentCalendar;
		}



		db::DBTransaction CalendarOGTFileFormat::Importer_::_save() const
		{
			// Save modified data in OGTFileFormant in transaction
			DBTransaction transaction = _ogtImporter._save();

			// Save calendars
			BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& element, _calendarElementsToRemove)
			{
				DBModule::GetDB()->deleteStmt(element->getKey(), transaction);
			}
			//_calendarElementsToRemove.clear();

			BOOST_FOREACH(const Registry<CalendarTemplate>::value_type calendar, _env.getRegistry<CalendarTemplate>())
			{
				CalendarTemplateTableSync::Save(calendar.second.get(), transaction);
				//CalendarTemplateElementTableSync::Clean(calendar.second->getKey(), transaction);
			}
			BOOST_FOREACH(const Registry<CalendarTemplateElement>::value_type calendarElement, _env.getRegistry<CalendarTemplateElement>())
			{
				CalendarTemplateElementTableSync::Save(calendarElement.second.get(), transaction);
			}
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
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_ogtImporter(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		bool CalendarOGTFileFormat::Importer_::beforeParsing()
		{
			return PTDataCleanerFileFormat::beforeParsing()
				|| _ogtImporter.beforeParsing();
		}



		bool CalendarOGTFileFormat::Importer_::afterParsing()
		{
			return _ogtImporter.afterParsing() || PTDataCleanerFileFormat::beforeParsing();
		}



		util::ParametersMap CalendarOGTFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result(PTDataCleanerFileFormat::_getParametersMap());
			return result;
		}



		void CalendarOGTFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			// Keep default value. CalendarTemplateElement clean up is required.
			//_cleanOldData = true;
		}
}	}
