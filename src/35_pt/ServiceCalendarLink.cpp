
/** ServiceCalendarLink class implementation.
	@file ServiceCalendarLink.cpp

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

#include "ServiceCalendarLink.hpp"
#include "Calendar.h"
#include "CalendarTemplate.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace pt;
	using namespace calendar;

	namespace util
	{
		template<> const string Registry<ServiceCalendarLink>::KEY("ServiceCalendarLink");
	}

	namespace pt
	{
		ServiceCalendarLink::ServiceCalendarLink(
			RegistryKeyType id
		):	Registrable(id),
			_service(NULL),
			_calendarTemplate(NULL),
			_calendarTemplate2(NULL)
		{}



		void ServiceCalendarLink::addDatesToCalendar(calendar::Calendar& cal) const
		{
			// Base calendar
			Calendar result;
			if(!_startDate.is_not_a_date() && !_endDate.is_not_a_date())
			{
				result = Calendar(_startDate, _endDate);
			}
			else if(_calendarTemplate && _calendarTemplate->isLimited())
			{
				result = _calendarTemplate->getResult();
			}
			else if(_calendarTemplate2 && _calendarTemplate2->isLimited())
			{
				result = _calendarTemplate2->getResult();
			}
			else
			{
				return; // Result is not limited : unable to build a calendar
			}

			// Apply templates masks
			if(_calendarTemplate)
			{
				result = _calendarTemplate->getResult(result);
			}
			if(_calendarTemplate2)
			{
				result = _calendarTemplate2->getResult(result);
			}

			// Add dates to the retval
			cal |= result;
		}
}	}
