
/** TimetableWarning class implementation.
	@file TimetableWarning.cpp

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

#include "TimetableWarning.h"

#include "CalendarTemplate.h"
#include "ParametersMap.h"

using namespace boost;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace util;

	namespace timetables
	{
		const string TimetableWarning::DATA_NUMBER("number");
		const string TimetableWarning::DATA_TEXT("text");
		const string TimetableWarning::DATA_FIRST_YEAR("first_year");
		const string TimetableWarning::DATA_FIRST_MONTH("first_month");
		const string TimetableWarning::DATA_FIRST_DAY("first_day");
		const string TimetableWarning::DATA_LAST_YEAR("last_year");
		const string TimetableWarning::DATA_LAST_MONTH("last_month");
		const string TimetableWarning::DATA_LAST_DAY("last_day");
		const string TimetableWarning::DATA_FIRST_DATE = "first_date";
		const string TimetableWarning::DATA_LAST_DATE = "last_date";
		const string TimetableWarning::TAG_DAY = "day";
		const string TimetableWarning::ATTR_DATE = "date";
		const string TimetableWarning::TAG_CALENDAR = "calendar";



		TimetableWarning::TimetableWarning(
			const Calendar& calendar,
			size_t number,
			const string& text,
			const CalendarTemplate* calendarTemplate
		):	_number(number),
			_calendar(calendar),
		  _text(text),
			_calendarTemplate(calendarTemplate)
		{}



		size_t TimetableWarning::getNumber() const
		{
			return _number;
		}



		const Calendar& TimetableWarning::getCalendar() const
		{
			return _calendar;
		}



		const std::string& TimetableWarning::getText() const
		{
			return _text;
		}



		void TimetableWarning::toParametersMap(
			ParametersMap& pm,
			bool withDates
		) const	{

			pm.insert(DATA_NUMBER, getNumber());
			pm.insert(DATA_TEXT, getText());

			// Active days
			const Calendar& calendar(getCalendar());
			date firstDate(calendar.getFirstActiveDate());
			date lastDate(calendar.getLastActiveDate());
			if(withDates)
			{
				for(date day(firstDate); day <lastDate; day += days(1))
				{
					// Jump over inactive days
					if(!calendar.isActive(day))
					{
						continue;
					}

					// Writing the active date to a sub map
					boost::shared_ptr<ParametersMap> datePM(new ParametersMap);
					datePM->insert(ATTR_DATE, day);
					pm.insert(TAG_DAY, datePM);
			}	}

			// Active days bounds
			pm.insert(DATA_FIRST_DATE, firstDate);
			pm.insert(DATA_LAST_DATE, firstDate);
			
			// Active days bounds (old schema - deprecated)
			pm.insert(DATA_FIRST_DAY, firstDate.day());
			pm.insert(DATA_FIRST_MONTH, firstDate.month());
			pm.insert(DATA_FIRST_YEAR, firstDate.year());
			pm.insert(DATA_LAST_DAY, lastDate.day());
			pm.insert(DATA_LAST_MONTH, lastDate.month());
			pm.insert(DATA_LAST_YEAR, lastDate.year());

			// Calendar template
			if(_calendarTemplate)
			{
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				_calendarTemplate->toParametersMap(*calendarPM);
				pm.insert(TAG_CALENDAR, calendarPM);
			}
		}
}	}
