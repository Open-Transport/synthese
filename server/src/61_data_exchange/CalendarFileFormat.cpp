
/** CalendarFileFormat class implementation.
	@file CalendarFileFormat.cpp

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

#include "CalendarFileFormat.hpp"

#include "CalendarTemplate.h"
#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateTableSync.h"
#include "DataSource.h"
#include "DBModule.h"
#include "Import.hpp"
#include "Importer.hpp"
#include "stddef.h"

#include <set>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace impex;
	using namespace util;
	
	namespace data_exchange
	{
		CalendarFileFormat::CalendarFileFormat(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		CalendarFileFormat::~CalendarFileFormat() { };



		CalendarTemplate* CalendarFileFormat::_getCalendarTemplate(
			ImportableTableSync::ObjectBySource<CalendarTemplateTableSync>& calendars,
			const std::string& code
		) const {
			CalendarTemplate* result = NULL;
			// Search for a calendar template linked with the datasource
			ImportableTableSync::ObjectBySource<CalendarTemplateTableSync>::Set
				loadedCalendars(calendars.get(code));

			if(loadedCalendars.empty())
			{
				_logWarning(
					"No calendar template with key "+ code
				);
			}
			else
			{
				if(loadedCalendars.size() > 1)
				{
					_logWarning(
						"More than one calendar template with key "+ code
					);
				}
				result = *loadedCalendars.begin();

				CalendarTemplateElementTableSync::Search(_env, result->getKey());
			}
			return result;
		}



		boost::shared_ptr<calendar::CalendarTemplate> CalendarFileFormat::_createCalendarTemplate(
			impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync>& calendars,
			const std::string& name,
			const std::string& code,
			const impex::DataSource& source,
			boost::optional<DatesVector> dateList
		) const {
			// Object creation
			boost::shared_ptr<calendar::CalendarTemplate> calendar(
				new CalendarTemplate(CalendarTemplateTableSync::getId())
			);
			calendar->setName(name);

			Importable::DataSourceLinks links;
			links.insert(make_pair(&source, code));
			calendar->setDataSourceLinksWithoutRegistration(links);
			_env.getEditableRegistry<CalendarTemplate>().add(calendar);
			calendars.add(*calendar);

			// Log
			stringstream logStream;
			logStream << "Creation of the calendar template with key " << code << " (" << name <<  ")";
			_logCreation(logStream.str());

			// Insert calendar template elements for each day in dateList
			int rank(0);
			if (dateList.is_initialized() && !dateList->empty()) {
				BOOST_FOREACH(const date aDate, dateList.get())
				{
					_createCalendarTemplateElement(calendar, rank, aDate);
					rank++;
				}
			}
			_logInfo("Calendar " + name + " created with "
				+ boost::lexical_cast<std::string>(rank) + " dates"
			);

			return calendar;
		}



		boost::shared_ptr<calendar::CalendarTemplateElement> CalendarFileFormat::_createCalendarTemplateElement(
				boost::shared_ptr<calendar::CalendarTemplate> calendar,
				const size_t rank,
				const boost::gregorian::date aDate
		) const {
			boost::shared_ptr<CalendarTemplateElement> element(
				new CalendarTemplateElement(CalendarTemplateElementTableSync::getId())
			);
			element->setMinDate(aDate);
			element->setMaxDate(aDate);
			element->setStep(boost::gregorian::days(1));

			element->setRank(rank);
			element->setOperation(CalendarTemplateElement::ADD);
			element->setCalendar(calendar.get());
			calendar->addElement(const_cast<CalendarTemplateElement&>(*element));
			// Insert in registry
			_env.getEditableRegistry<CalendarTemplateElement>().add(element);
			return element;
		}



		void CalendarFileFormat::_removeCalendarTemplateElement(
				boost::shared_ptr<calendar::CalendarTemplate> calendar,
				boost::shared_ptr<calendar::CalendarTemplateElement> oldElement
		) const {
			_calendarElementsToRemove.insert(oldElement);
			_env.getEditableRegistry<CalendarTemplateElement>().remove(oldElement->getKey());
			calendar->removeElement(*oldElement);
		}



		boost::shared_ptr<calendar::CalendarTemplate> CalendarFileFormat::_createOrUpdateCalendarTemplate(
			impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync>& calendars,
			const std::string& name,
			const std::string& code,
			const impex::DataSource& source,
			boost::optional<DatesVector> dateList
		) const {
			calendar::CalendarTemplate* calPointer = _getCalendarTemplate(calendars, code);
			if (!calPointer) {
				return _createCalendarTemplate(calendars, name, code, source, dateList);
			}
			// Do not use the CalendarTemplate in another shared_ptr: double free segfault
			// Instead get shared_ptr from Env/Registry
			boost::shared_ptr<calendar::CalendarTemplate> result(
					CalendarTemplateTableSync::GetEditable(calPointer->getKey(), _env));

			if (!dateList || dateList->empty()) {
				return result;
			}

			// First naive non optimal implementation. Objective: limit changes on DB/intersynthese
			size_t maxRank(0);

			std::set<date> workingList(dateList->begin(), dateList->end());

			CalendarTemplateElementTableSync::SearchResult elements(
				CalendarTemplateElementTableSync::Search(_env, result->getKey()));

			BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& element, elements)
			{
				if (element->getStep().days() != 1
					|| element->getMinDate() != element->getMaxDate())
				{
					_logError("Abort. Unexpected CalendarTemplateElement " + element->getKey());
					boost::shared_ptr<CalendarTemplate> empty;
					return empty;
				}

				if (workingList.find(element->getMinDate()) == workingList.end()) {
					// Element date not found in new dateList
					_removeCalendarTemplateElement(result, element);
				}

				if (element->getRank() > maxRank)
				{
					maxRank = element->getRank();
				}
			}

			// Create a CalendarTemplateElement for lacking dates
			const Calendar effectiveCalendar = result->getResult();
			BOOST_FOREACH(const date aDate, dateList.get()) {
				if (!effectiveCalendar.isActive(aDate)) {
					_createCalendarTemplateElement(result, ++maxRank, aDate);
				}
			}

			// Finally update CalendarTemplate attributes
			result->setName(name);

			return result;
		}



		void CalendarFileFormat::_saveCalendarTemplates(db::DBTransaction& transaction) const
		{
			BOOST_FOREACH(const Registry<CalendarTemplate>::value_type& calendarTemplate, _env.getRegistry<CalendarTemplate>())
			{
				CalendarTemplateTableSync::Save(calendarTemplate.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<CalendarTemplateElement>::value_type& calendarTemplateElement, _env.getRegistry<CalendarTemplateElement>())
			{
				CalendarTemplateElementTableSync::Save(calendarTemplateElement.second.get(), transaction);
			}
			// For some reasons, removal must be done last, because previous loop create elements again
			BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& element, _calendarElementsToRemove)
			{
				db::DBModule::GetDB()->deleteStmt(element->getKey(), transaction);
			}
		}

}	}

