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
			// If time is not defined it does not belong to this application period
			if(time.is_not_a_date_time())
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

			// If start hour and end hour are defined, check time against them
			if(!get<StartHour>().is_not_a_date_time() && !get<EndHour>().is_not_a_date_time())
			{
				// If StartHour is posterior to EndHour the interval is empty and time can never belong to it
				if(get<StartHour>() > get<EndHour>())
				{
					return false;
				}

				else
				{
					posix_time::time_duration timeOfDay = time.time_of_day();

					// If [StartHour, EndHour] interval spans over 2 days, translate timeOfDay by the necessary amount of days
					// Example : if {Start,End}Hour = [23:00 ; 25:00] the message must be broadcast from 23:00 to 01:00 the next day
					// 00:00 is inside the interval, but 00:00 < 23:00 so a straight comparison would reject this date
					// we add one full day so that time > StartHour : 24:00 E [23:00 ; 25:00] => this method will return true
					if((24 <= get<StartHour>().hours()) || (24 <= get<EndHour>().hours()))
					{
						if(timeOfDay < get<StartHour>())
						{
							// Compute time difference between time and StartHour, and convert this difference into a number of days, rounded upward
							posix_time::time_duration deltaTimeToStart = get<StartHour>() - timeOfDay;
							posix_time::time_duration timeTranslator = posix_time::hours((deltaTimeToStart.hours() / 24) * 24);

							if(timeTranslator < deltaTimeToStart)
							{
								timeTranslator += posix_time::hours(24);
							}

							// Translate timeOfDay so that it is now superior or equal to StartHour
							timeOfDay += timeTranslator;
						}
					}

					// Check that timeOfDay belongs to [StartHour, EndHour]
					if((timeOfDay < get<StartHour>()) || (timeOfDay > get<EndHour>()))
					{
						return false;
					}

					return true;
				}
			}

			/* This part does not work with {Start,End}Hour values above 24h
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
			*/

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

					// Append { date or today } and StartHour
					result = posix_time::ptime(result.date(), get<StartHour>());
				}

				else
				{
					// StartTime and StartHour are defined, combine them and return max(StartTime, StartTime.date + StartHour)
					posix_time::ptime startTimeAndHour(get<StartTime>().date(), get<StartHour>());
					result = (get<StartTime>() > startTimeAndHour) ? get<StartTime>() : startTimeAndHour;
				}
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

					// Append { date or today } and EndHour
					result = posix_time::ptime(result.date(), get<EndHour>());
				}

				else
				{
					// EndTime and EndHour are defined, combine them and return min(EndTime, EndTime.date + EndHour)
					posix_time::ptime endTimeAndHour(get<EndTime>().date(), get<EndHour>());
					result = (get<EndTime>() > endTimeAndHour) ? endTimeAndHour : get<EndTime>();
				}
			}
			return result;
		}


		bool MessageApplicationPeriod::isAfter( const boost::posix_time::ptime& time ) const
		{
			// Undefined time is neither before nor after an application period
			if(time.is_not_a_date_time())
			{
				return false;
			}

			// If EndTime is undefined, this application period goes on forever
			if(get<EndTime>().is_not_a_date_time())
			{
				return false;
			}

			// Quick rejection test : if time > EndTime, it is after the application period
			if(time > get<EndTime>())
			{
				return true;
			}

			// Compute the real upper boundary of this application period and compare it with time
			return (time > getEnd());
		}

}	}
