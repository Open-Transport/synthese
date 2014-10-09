
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
#include "ScenarioCalendarTableSync.hpp"
#include "ScenarioTableSync.h"
#include "SentScenario.h"

using namespace boost;
using namespace std;

namespace synthese
{
	CLASS_DEFINITION(messages::ScenarioCalendar, "t110_scenario_calendars", 110)
	FIELD_DEFINITION_OF_OBJECT(messages::ScenarioCalendar, "scenario_calendar_id", "scenario_calendar_ids")
	FIELD_DEFINITION_OF_TYPE(ScenarioPointer, "scenario_id", SQL_INTEGER)

	using namespace util;
	using namespace db;

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
				shared_ptr<ParametersMap> apPM(new ParametersMap);
				ap->toParametersMap(*apPM, false, false, prefix);
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

		synthese::SubObjects ScenarioCalendar::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(const ApplicationPeriods::value_type& applicationPeriod, getApplicationPeriods())
			{
				r.push_back(const_cast<MessageApplicationPeriod*>(applicationPeriod));
			}
			return r;
		}

		void ScenarioCalendar::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix /*= std::string() */
		) const	{
			// Inter synthese package
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + SimpleObjectFieldDefinition<Name>::FIELD.name,
				get<Name>()
			);
			pm.insert(
				prefix + SimpleObjectFieldDefinition<ScenarioPointer>::FIELD.name,
				get<ScenarioPointer>() ? get<ScenarioPointer>()->getKey() : RegistryKeyType(0)
			);
		}

		bool ScenarioCalendar::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Name
			if(record.isDefined(SimpleObjectFieldDefinition<Name>::FIELD.name))
			{
				string value(
					record.get<string>(SimpleObjectFieldDefinition<Name>::FIELD.name)
				);
				if(value != get<Name>())
				{
					set<Name>(value);
					result = true;
				}
			}

			// Scenario
			if(record.isDefined(SimpleObjectFieldDefinition<ScenarioPointer>::FIELD.name))
			{
				Scenario* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						SimpleObjectFieldDefinition<ScenarioPointer>::FIELD.name,
						0
				)	);
				if(id > 0)
				{
					try
					{
						value = ScenarioTableSync::GetEditable(id, env).get();
					}
					catch(ObjectNotFoundException<Scenario>&)
					{
						Log::GetInstance().warn("No such scenario in scenario calendar "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != get<ScenarioPointer>().get_ptr())
				{
					set<ScenarioPointer>(*value);
					result = true;
				}
			}

			return result;
		}
}	}

