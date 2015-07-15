
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

#include "Alarm.h"
#include "SentScenario.h"

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
					FIELD_DEFAULT_CONSTRUCTOR(EndTime),
					FIELD_DEFAULT_CONSTRUCTOR(Dates)
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



		bool MessageApplicationPeriod::getValue( const boost::posix_time::ptime& time ) const
		{
			// If dates are defined, the current time must belong to the dates list
			if(	!Calendar::empty() && !isActive(time.date()))
			{
				return false;
			}
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



		boost::posix_time::ptime MessageApplicationPeriod::getStart(
			const boost::posix_time::ptime& date /* = boost::posix_time::not_a_date_time */
		) const
		{
			posix_time::ptime result = posix_time::not_a_date_time;

			if (!get<StartTime>().is_not_a_date_time())
			{
				result = get<StartTime>();
			}
			if (!get<StartHour>().is_not_a_date_time())
			{
				if (result.is_not_a_date_time())
				{
					if (date.is_not_a_date_time())
					{
						// Get today date
						result = posix_time::second_clock::local_time();
					}
					else
					{
						result = date;
					}
				}
				// Append { StartTime or date or today } and StartHour
				result = posix_time::ptime(result.date(), get<StartHour>());
			}
			return result;
		}



		boost::posix_time::ptime MessageApplicationPeriod::getEnd(
			const boost::posix_time::ptime& date /* = boost::posix_time::not_a_date_time */
		) const
		{
			posix_time::ptime result = posix_time::not_a_date_time;

			if (!get<EndTime>().is_not_a_date_time())
			{
				result = get<EndTime>();
			}
			if (!get<EndHour>().is_not_a_date_time())
			{
				if (result.is_not_a_date_time())
				{
					if (date.is_not_a_date_time())
					{
						// Get today date
						result = posix_time::second_clock::local_time();
					}
					else
					{
						result = date;
					}
				}
				// Append { EndTime or date or today } and EndHour
				result = posix_time::ptime(result.date(), get<EndHour>());
			}
			return result;
		}



		void MessageApplicationPeriod::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			BOOST_FOREACH(const date& d, getActiveDates())
			{
				boost::shared_ptr<ParametersMap> datePM(new ParametersMap);
				datePM->insert(TAG_DATE, to_iso_extended_string(d));
				map.insert(TAG_DATE, datePM);
			}
		}
}	}
