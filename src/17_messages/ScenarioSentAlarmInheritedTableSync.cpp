
/** ScenarioSentAlarmInheritedTableSync class implementation.
	@file ScenarioSentAlarmInheritedTableSync.cpp

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

#include "ScenarioSentAlarmInheritedTableSync.h"
#include "SentScenarioInheritedTableSync.h"
#include "Env.h"
#include <sstream>

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;

	template<>
	const string util::FactorableTemplate<AlarmTableSync, ScenarioSentAlarmInheritedTableSync>::FACTORY_KEY("ScenarioSentAlarmInheritedTableSync");

	namespace db
	{


		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,ScenarioSentAlarmInheritedTableSync,ScenarioSentAlarm>::Load(
			ScenarioSentAlarm* obj,
			const SQLiteResultSPtr& rows, 
			Env* env,
			LinkLevel linkLevel
		){
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				obj->setScenario(
					SentScenarioInheritedTableSync::Get(
						rows->getLongLong(AlarmTableSync::COL_SCENARIO_ID)
					).get()
				);
			}
		}



		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,ScenarioSentAlarmInheritedTableSync,ScenarioSentAlarm>::Unlink(ScenarioSentAlarm* obj, Env* env)
		{
			obj->setScenario(NULL);
		}


		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,ScenarioSentAlarmInheritedTableSync,ScenarioSentAlarm>::Save(ScenarioSentAlarm* obj)
		{
			if (obj->getKey() == UNKNOWN_VALUE)
				obj->setKey(getId());
			stringstream query;		
			query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(obj->getKey())
				<< ",0"
				<< "," << Conversion::ToString(obj->getIsEnabled())
				<< "," << Conversion::ToString((int) obj->getLevel())
				<< "," << Conversion::ToSQLiteString(obj->getShortMessage())
				<< "," << Conversion::ToSQLiteString(obj->getLongMessage())
				<< "," << obj->getPeriodStart().toSQLString()
				<< "," << obj->getPeriodEnd().toSQLString()
				<< "," << Conversion::ToString(obj->getScenario()->getKey())
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}

	namespace messages
	{

		ScenarioSentAlarmInheritedTableSync::ScenarioSentAlarmInheritedTableSync()
			: SQLiteInheritedRegistryTableSync<AlarmTableSync, ScenarioSentAlarmInheritedTableSync, ScenarioSentAlarm>()
		{

		}



		void ScenarioSentAlarmInheritedTableSync::Search( util::Env& env, const SentScenario* scenario , int first /*= 0 */, int number /*= 0 */, bool orderByLevel /*= false */, bool orderByStatus /*= false */, bool orderByConflict /*= false */, bool raisingOrder /*= false*/, util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */ )
		{
			stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE_NAME << " AS a "
				<< " WHERE "
				<< COL_IS_TEMPLATE << "=0"
				<< " AND " << COL_SCENARIO_ID << "=" << scenario->getKey();
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
