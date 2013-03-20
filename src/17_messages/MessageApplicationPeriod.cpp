
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

#include "SentAlarm.h"
#include "SentScenario.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(MessageApplicationPeriod, "t104_message_application_periods", 104)

	FIELD_DEFINITION_OF_TYPE(AlarmPointer, "message_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ScenarioPointer, "scenario_id", SQL_INTEGER)
    FIELD_DEFINITION_OF_TYPE(Dates, "dates", SQL_TEXT)

	namespace messages
	{
		MessageApplicationPeriod::MessageApplicationPeriod(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<MessageApplicationPeriod, MessageApplicationPeriodRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(AlarmPointer),
					FIELD_DEFAULT_CONSTRUCTOR(ScenarioPointer),
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
			optional<Alarm&> message(get<AlarmPointer>());
			if(message)
			{
				ApplicationPeriods ap(dynamic_cast<SentAlarm&>(*message).getApplicationPeriods());
				ap.insert(
					make_pair(
						make_pair(get<StartHour>(), get<EndHour>()),
						this
				)	);
				dynamic_cast<SentAlarm&>(*message).setApplicationPeriods(ap);
			}
			else
			{
				optional<Scenario&> scenario(get<ScenarioPointer>());
				if(scenario)
				{
					ApplicationPeriods ap(dynamic_cast<SentScenario&>(*scenario).getApplicationPeriods());
					ap.insert(
						make_pair(
						make_pair(get<StartHour>(), get<EndHour>()),
							this
					)	);
					dynamic_cast<SentScenario&>(*scenario).setApplicationPeriods(ap);
				}
			}
		}



		void MessageApplicationPeriod::unlink()
		{
			optional<Alarm&> message(get<AlarmPointer>());
			if(message)
			{
				ApplicationPeriods ap(dynamic_cast<SentAlarm&>(*message).getApplicationPeriods());
				for(ApplicationPeriods::iterator it(ap.begin()); it != ap.end(); ++it)
				{
					if(it->second == this)
					{
						ap.erase(it);
					}
				}
				dynamic_cast<SentAlarm&>(*message).setApplicationPeriods(ap);
			}
			else
			{
				optional<Scenario&> scenario(get<ScenarioPointer>());
				if(scenario)
				{
					ApplicationPeriods ap(dynamic_cast<SentScenario&>(*scenario).getApplicationPeriods());
					for(ApplicationPeriods::iterator it(ap.begin()); it != ap.end(); ++it)
					{
						if(it->second == this)
						{
							ap.erase(it);
						}
					}
					dynamic_cast<SentScenario&>(*scenario).setApplicationPeriods(ap);
				}
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Checks if the specified time is in the defined period.
		/// @param time the time to check
		/// @return true if the time is in the defined period
		bool MessageApplicationPeriod::getValue( const boost::posix_time::ptime& time ) const
		{
			return isActive(time.date()) && time.time_of_day() > get<StartHour>() && time.time_of_day() < get<EndHour>();
		}
}	}
