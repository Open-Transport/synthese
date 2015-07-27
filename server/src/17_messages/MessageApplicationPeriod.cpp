/** MessageApplicationPeriod class implementation.
	@file MessageApplicationPeriod.cpp

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

#include "MessageApplicationPeriod.hpp"

using namespace boost;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(MessageApplicationPeriod, "t104_message_application_periods", 104)

	namespace messages
	{
		const string MessageApplicationPeriod::TAG_DATE = "date";


		MessageApplicationPeriod::MessageApplicationPeriod(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<MessageApplicationPeriod, MessageApplicationPeriodRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(ScenarioCalendar),
					FIELD_DEFAULT_CONSTRUCTOR(StartHour),
					FIELD_DEFAULT_CONSTRUCTOR(EndHour),
					FIELD_DEFAULT_CONSTRUCTOR(StartTime),
					FIELD_DEFAULT_CONSTRUCTOR(EndTime)
			)	)
		{}




		void MessageApplicationPeriod::link(
			util::Env& env,
			bool withAlgorithmOptimizations /*= false*/
		){
			if(get<ScenarioCalendar>())
			{
				ScenarioCalendar::ApplicationPeriods ap(get<ScenarioCalendar>()->getApplicationPeriods());
				ap.insert(this);
				get<ScenarioCalendar>()->setApplicationPeriods(ap);
			}
		}



		void MessageApplicationPeriod::unlink()
		{
			if(get<ScenarioCalendar>())
			{
				ScenarioCalendar::ApplicationPeriods ap(get<ScenarioCalendar>()->getApplicationPeriods());
				ScenarioCalendar::ApplicationPeriods::iterator it(ap.find(this));
				if(it != ap.end())
				{
					ap.erase(it);
				}
				get<ScenarioCalendar>()->setApplicationPeriods(ap);
			}
		}



		bool MessageApplicationPeriod::isInside( const boost::posix_time::ptime& time ) const
		{
			// If start time is defined, the current time must be after it
			if(	!get<StartTime>().is_not_a_date_time() && time < get<StartTime>())
			{
				return false;
			}
			// If end time is defined, the current time must be before it
			if(	!get<EndTime>().is_not_a_date_time() && time > get<EndTime>())
			{
				return false;
			}
			// If start hour is defined, the current time must be after it
			if(	!get<StartHour>().is_not_a_date_time() && time.time_of_day() < get<StartHour>())
			{
				return false;
			}
			// If end hour is defined, the current time must be before it
			if(	!get<EndHour>().is_not_a_date_time() && time.time_of_day() > get<EndHour>())
			{
				return false;
			}

			// No filter has been activated, then the message must be displayed
			return true;
		}

		bool MessageApplicationPeriod::isAfter( const boost::posix_time::ptime& time ) const
		{
			if(time > get<EndTime>())
			{
				return true;
			}
			if (!get<EndHour>().is_not_a_date_time())
			{
				date endTimeDate(get<EndTime>().date());
				boost::posix_time::ptime lastEndHour(endTimeDate, get<EndHour>());
				if (time > lastEndHour) return true;
			}
			return false;
		}

}	}
