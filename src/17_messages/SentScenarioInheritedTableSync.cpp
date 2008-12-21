
/** SentScenarioInheritedTableSync class implementation.
	@file SentScenarioInheritedTableSync.cpp

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

#include "SentScenarioInheritedTableSync.h"
#include "SentScenario.h"

#include "ScenarioInheritedTableSync.h"
#include "DateTime.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;
	using namespace time;
	

	template<>
	const string util::FactorableTemplate<ScenarioTableSync,SentScenarioInheritedTableSync>::FACTORY_KEY("SentScenarioInheritedTableSync");

	namespace db
	{
		
		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Load(
			SentScenario* obj,
			const SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);

			obj->setIsEnabled(rows->getBool ( ScenarioTableSync::COL_ENABLED));
			obj->setPeriodStart(DateTime::FromSQLTimestamp (rows->getText ( ScenarioTableSync::COL_PERIODSTART)));
			obj->setPeriodEnd(DateTime::FromSQLTimestamp (rows->getText ( ScenarioTableSync::COL_PERIODEND)));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				LoadScenarioAlarms<SentScenario>(obj);
			}
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Unlink(
			SentScenario* obj,
			Env* env
		){

		}

		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Save(
			SentScenario* obj
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(obj->getKey())
				<< ",0"
				<< "," << Conversion::ToString(obj->getIsEnabled())
				<< "," << Conversion::ToSQLiteString(obj->getName())
				<< "," << obj->getPeriodStart().toSQLString()
				<< "," << obj->getPeriodEnd().toSQLString()
				<< "," << UNKNOWN_VALUE
				<< ")";
			sqlite->execUpdate(query.str());

			stringstream alarmquery;
			alarmquery
				<< "UPDATE "
				<< AlarmTableSync::TABLE.NAME
				<< " SET "
				<< AlarmTableSync::COL_PERIODSTART << "=" << obj->getPeriodStart().toSQLString()
				<< "," << AlarmTableSync::COL_PERIODEND << "=" << obj->getPeriodEnd().toSQLString()
				<< " WHERE " 
				<< AlarmTableSync::COL_SCENARIO_ID << "=" << obj->getKey();
			sqlite->execUpdate(alarmquery.str());
		}
	}

	namespace messages
	{

		SentScenarioInheritedTableSync::SentScenarioInheritedTableSync()
			: SQLiteInheritedRegistryTableSync<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>()
		{

		}

		void SentScenarioInheritedTableSync::Search(
			Env& env,
			time::DateTime startDate
			, time::DateTime endDate
			, const std::string name
			, int first /*= 0*/, int number /*= 0*/
			, bool orderByDate
			, bool orderByName
			, bool orderByStatus
			, bool orderByConflict
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
				<< COL_IS_TEMPLATE << "=0";
			if (!startDate.isUnknown())
				query << " AND " << ScenarioTableSync::COL_PERIODEND << "<=" << startDate.toSQLString();
			if (!endDate.isUnknown())
				query << " AND " << ScenarioTableSync::COL_PERIODSTART << "<=" << endDate.toSQLString();
			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToSQLiteString(name);
			if (orderByDate)
				query << " ORDER BY " << COL_PERIODSTART << (raisingOrder ? " ASC" : " DESC") << "," << COL_PERIODEND  << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
