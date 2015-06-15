
/** ScenarioCalendarTableSync class implementation.
	@file ScenarioCalendarTableSync.cpp

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

#include "ScenarioCalendarTableSync.hpp"

#include "SentScenario.h"
#include "AlarmTableSync.h"
#include "DBResult.hpp"
#include "SelectQuery.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace messages;

	namespace util
	{
		template<>
		const string FactorableTemplate<DBTableSync, ScenarioCalendarTableSync>::FACTORY_KEY = "17.40 Scenario calendars";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ScenarioCalendarTableSync>::TABLE(
			"t110_scenario_calendars"
		);

		template<> const Field DBTableSyncTemplate<ScenarioCalendarTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ScenarioCalendarTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<ScenarioCalendarTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<ScenarioCalendarTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ScenarioCalendarTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ScenarioCalendarTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace messages
	{

		ScenarioCalendarTableSync::SearchResult ScenarioCalendarTableSync::Search(
			Env& env,
			optional<RegistryKeyType> scenarioId,
			int first /*= 0*/,
			optional<size_t> number,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<ScenarioCalendarTableSync> query;
			if (scenarioId)
			{
				query.addWhereField(SimpleObjectFieldDefinition<ScenarioPointer>::FIELD.name, *scenarioId, ComposedExpression::OP_EQ);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
