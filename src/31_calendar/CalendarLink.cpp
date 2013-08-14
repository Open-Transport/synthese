
/** CalendarLink class implementation.
	@file CalendarLink.cpp

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

#include "CalendarLink.hpp"

#include "Calendar.h"
#include "CalendarTemplate.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace calendar;

	namespace util
	{
		template<> const string Registry<CalendarLink>::KEY("CalendarLink");
	}

	namespace calendar
	{
		CalendarLink::CalendarLink(
			RegistryKeyType id
		):	Registrable(id),
			_calendar(NULL),
			_calendarTemplate(NULL),
			_calendarTemplate2(NULL)
		{}



		void CalendarLink::addDatesToBitSets(Calendar::BitSets& bitsets) const
		{
			// Do nothing if both calendar templates are null
			if(!_calendarTemplate && !_calendarTemplate2)
			{
				return;
			}

			// Base calendar
			Calendar mask;
			if(!_startDate.is_not_a_date() && !_endDate.is_not_a_date())
			{
				mask = Calendar(_startDate, _endDate);
			}
			else if(_calendarTemplate && _calendarTemplate->isLimited())
			{
				mask = _calendarTemplate->getResult();
			}
			else if(_calendarTemplate2 && _calendarTemplate2->isLimited())
			{
				mask = _calendarTemplate2->getResult();
			}
			else
			{
				return; // Result is not limited : unable to build a calendar
			}

			// Apply templates masks
			if(_calendarTemplate)
			{
				Calendar result(_calendarTemplate->getResult(mask));
				if(_calendarTemplate2)
				{
					bitsets |= _calendarTemplate2->getResult(result)._getDatesCache();
				}
				else
				{
					bitsets |= result._getDatesCache();
				}
			}
			else
			{
				bitsets |= _calendarTemplate2->getResult(mask)._getDatesCache();
			}
		}
}	}
