
/** StopPointAccessibilityCalendar class implementation.
	@file StopPointAccessibilityCalendar.cpp

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

#include "StopPointInaccessibilityCalendar.hpp"

#include "AccessParameters.h"

namespace synthese
{
	using namespace util;
	using namespace calendar;
	using namespace pt;

	CLASS_DEFINITION(StopPointInaccessibilityCalendar, "t501_stop_point_inaccessibility_calendars", 501)

	FIELD_DEFINITION_OF_TYPE(StopPointer, "stop_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CalendarTemplatePointer, "calendar_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(UserClass, "user_class", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DailyTimeFilter, "daily_time_filter", SQL_BOOLEAN)

	namespace pt
	{
		StopPointInaccessibilityCalendar::StopPointInaccessibilityCalendar(
			util::RegistryKeyType id
		):
			Registrable(id),
			Object<StopPointInaccessibilityCalendar, StopPointAccessibilityCalendarSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(StopPointer),
					FIELD_DEFAULT_CONSTRUCTOR(CalendarTemplatePointer),
					FIELD_DEFAULT_CONSTRUCTOR(UserClass),
					FIELD_VALUE_CONSTRUCTOR(StartHour, boost::posix_time::not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(EndHour, boost::posix_time::not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(DailyTimeFilter),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks)
			)	)
		{}



		bool StopPointInaccessibilityCalendar::isAccessibleAtTime(
			const graph::AccessParameters& accessParameters,
			const boost::posix_time::ptime& time
		) const {
			// If we have an accessibility mask for the current user class
			if(!get<UserClass>() || (graph::UserClassCode)(get<UserClass>()) == accessParameters.getUserClass())
			{
				// Check if this day is marked as inaccessible
				bool isDayInaccessible = _inaccessibilityCalendar.isActive(time.date());
				if(isDayInaccessible)
				{
					// If it is, check start and end hour bounds
					if(!get<StartHour>().is_not_a_date_time() && (
						get<DailyTimeFilter>() || _inaccessibilityCalendar.getFirstActiveDate() == time.date()
					)){
						isDayInaccessible &= (time.time_of_day() >= get<StartHour>());
					}

					if(!get<EndHour>().is_not_a_date_time() && (
						get<DailyTimeFilter>() || _inaccessibilityCalendar.getLastActiveDate() == time.date()
					)){
						isDayInaccessible &= (time.time_of_day() <= get<EndHour>());
					}
				}

				return !isDayInaccessible;
			}

			return true;
		}



		void StopPointInaccessibilityCalendar::link(
			util::Env& env,
			bool withAlgorithmOptimizations
		){
			if(get<StopPointer>() && get<CalendarTemplatePointer>())
			{
				if(get<CalendarTemplatePointer>()->isLimited())
				{
					_inaccessibilityCalendar = get<CalendarTemplatePointer>()->getResult();
				}
				else
				{
					Calendar mask(boost::gregorian::day_clock::local_day(), boost::gregorian::day_clock::local_day() + boost::gregorian::years(2));
					_inaccessibilityCalendar = get<CalendarTemplatePointer>()->getResult(mask);
				}
				get<StopPointer>()->addAccessibilityMaskEntry(this);
			}
		}



		void StopPointInaccessibilityCalendar::unlink()
		{
			if(get<StopPointer>())
			{
				get<StopPointer>()->removeAccessibilityMaskEntry(this);
			}
		}
	}
}
