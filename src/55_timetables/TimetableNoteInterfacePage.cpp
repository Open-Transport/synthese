
/** TimetableNoteInterfacePage class implementation.
	@file TimetableNoteInterfacePage.cpp
	@author Hugues
	@date 2009

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

#include "TimetableNoteInterfacePage.h"
#include "TimetableWarning.h"
#include "WebPage.h"
#include "WebPageDisplayFunction.h"
#include "StaticFunctionRequest.h"
#include "CalendarDateInterfacePage.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace timetables;
	using namespace calendar;
	using namespace server;
	using namespace transportwebsite;
	using namespace server;

	namespace timetables
	{
		const string TimetableNoteInterfacePage::DATA_NUMBER("number");
		const string TimetableNoteInterfacePage::DATA_TEXT("text");
		const string TimetableNoteInterfacePage::DATA_CALENDAR("calendar");
		const string TimetableNoteInterfacePage::DATA_FIRST_YEAR("first_year");
		const string TimetableNoteInterfacePage::DATA_FIRST_MONTH("first_month");
		const string TimetableNoteInterfacePage::DATA_FIRST_DAY("first_day");
		const string TimetableNoteInterfacePage::DATA_LAST_YEAR("last_year");
		const string TimetableNoteInterfacePage::DATA_LAST_MONTH("last_month");
		const string TimetableNoteInterfacePage::DATA_LAST_DAY("last_day");



		void TimetableNoteInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const WebPage> page,
			boost::shared_ptr<const transportwebsite::WebPage> calendarDatePage,
			const server::Request& request,
			const TimetableWarning& object
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

		
			pm.insert(DATA_NUMBER, object.getNumber());
			pm.insert(DATA_TEXT, object.getText());

			if(calendarDatePage.get())
			{
				stringstream calendarContent;
				const Calendar& calendar(object.getCalendar());
				date firstDate(calendar.getFirstActiveDate().year(), calendar.getFirstActiveDate().month(), 1);
				date lastDate(calendar.getLastActiveDate().end_of_month());
				for(date day(firstDate); day <lastDate; day += days(1))
				{
					CalendarDateInterfacePage::Display(calendarContent, calendarDatePage, request, day, calendar.isActive(day));
				}
				pm.insert(DATA_CALENDAR, calendarContent.str());
				pm.insert(DATA_FIRST_DAY, firstDate.day());
				pm.insert(DATA_FIRST_MONTH, firstDate.month());
				pm.insert(DATA_FIRST_YEAR, firstDate.year());
				pm.insert(DATA_LAST_DAY, lastDate.day());
				pm.insert(DATA_LAST_MONTH, lastDate.month());
				pm.insert(DATA_LAST_YEAR, lastDate.year());
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
	}
}
