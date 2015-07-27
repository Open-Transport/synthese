
/** ScenarioCalendar class implementation.
	@file ScenarioCalendar.cpp

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

#include "ScenarioCalendar.hpp"

#include "MessageApplicationPeriodTableSync.hpp"
#include "ParametersMap.h"
#include "SentScenario.h"

using namespace boost;
using namespace std;

namespace synthese
{
	CLASS_DEFINITION(messages::ScenarioCalendar, "t110_scenario_calendars", 110)
	FIELD_DEFINITION_OF_OBJECT(messages::ScenarioCalendar, "scenario_calendar_id", "scenario_calendar_ids")
	FIELD_DEFINITION_OF_TYPE(ScenarioPointer, "scenario_id", SQL_INTEGER)

	using namespace util;

	namespace messages
	{
		const string ScenarioCalendar::TAG_APPLICATION_PERIOD = "application_period";



		ScenarioCalendar::ScenarioCalendar(
			util::RegistryKeyType id/*=0 */
		):	Registrable(id),
			Object<ScenarioCalendar, ScenarioCalendarRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(ScenarioPointer)
			)	)
		{}



		void ScenarioCalendar::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<ScenarioPointer>())
			{
				SentScenario::ScenarioCalendars ap(get<ScenarioPointer>()->getCalendars());
				ap.insert(this);
				get<ScenarioPointer>()->setCalendars(ap);
			}
		}



		void ScenarioCalendar::unlink()
		{
			if(get<ScenarioPointer>())
			{
				SentScenario::ScenarioCalendars ap(get<ScenarioPointer>()->getCalendars());
				SentScenario::ScenarioCalendars::iterator it(ap.find(this));
				if(it != ap.end())
				{
					ap.erase(it);
				}
				get<ScenarioPointer>()->setCalendars(ap);
			}
		}



		void ScenarioCalendar::addAdditionalParameters( util::ParametersMap& map, std::string prefix /*= std::string() */ ) const
		{
			BOOST_FOREACH(const ApplicationPeriods::value_type& ap, _applicationPeriods)
			{
				boost::shared_ptr<ParametersMap> apPM(new ParametersMap);
				ap->toParametersMap(*apPM);
				map.insert(TAG_APPLICATION_PERIOD, apPM);
			}
		}



		void ScenarioCalendar::beforeDelete( boost::optional<db::DBTransaction&> transaction ) const
		{
			// TODO transfort transaction into optional in Remove
			if(!transaction) return;
			// Application periods
			BOOST_FOREACH(MessageApplicationPeriod* period, _applicationPeriods)
			{
				MessageApplicationPeriodTableSync::Remove(NULL, period->getKey(), *transaction, false);
			}
		}

		bool ScenarioCalendar::isInside( const boost::posix_time::ptime& time ) const
		{
			BOOST_FOREACH(MessageApplicationPeriod* period, _applicationPeriods)
			{
				if (period->isInside(time))
				{
					return true;
				}
			}
			return false;
		}


		bool ScenarioCalendar::isAfter( const boost::posix_time::ptime& time ) const
		{
			if (_applicationPeriods.empty())
			{
				return false;
			}
			BOOST_FOREACH(MessageApplicationPeriod* period, _applicationPeriods)
			{
				if (!period->isAfter(time))
				{
					return false;
				}
			}
			return true;
		}


}	}

