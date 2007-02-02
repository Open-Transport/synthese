
/** ScenarioTableSync class implementation.
	@file ScenarioTableSync.cpp

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

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "04_time/DateTime.h"

#include "Scenario.h"
#include "ScenarioTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Scenario>::TABLE_NAME = "t039_scenarios";
		template<> const int SQLiteTableSyncTemplate<Scenario>::TABLE_ID = 39;
		template<> const bool SQLiteTableSyncTemplate<Scenario>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Scenario>::load(Scenario* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setIsATemplate(Conversion::ToBool(rows.getColumn(rowId, ScenarioTableSync::COL_IS_TEMPLATE)));
			object->setName(rows.getColumn(rowId, ScenarioTableSync::COL_NAME));
			object->setPeriodStart(DateTime::FromSQLTimestamp (rows.getColumn (rowId, ScenarioTableSync::COL_PERIODSTART)));
			object->setPeriodEnd(DateTime::FromSQLTimestamp (rows.getColumn (rowId, ScenarioTableSync::COL_PERIODEND)));
		}

		template<> void SQLiteTableSyncTemplate<Scenario>::save(Scenario* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					<< ScenarioTableSync::COL_IS_TEMPLATE << "=" << Conversion::ToString(object->getIsATemplate())
					<< "," << ScenarioTableSync::COL_NAME << "=" << Conversion::ToSQLiteString(object->getName())
					<< "," << ScenarioTableSync::COL_PERIODSTART << "=" << object->getPeriodStart().toSQLString()
					<< "," << ScenarioTableSync::COL_PERIODEND << "=" << object->getPeriodEnd().toSQLString()
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId(1,1));
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					<< "," << Conversion::ToString(object->getIsATemplate())
					<< "," << Conversion::ToSQLiteString(object->getName())
					<< "," << object->getPeriodStart().toSQLString()
					<< "," << object->getPeriodEnd().toSQLString()
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace messages
	{
		const std::string ScenarioTableSync::COL_IS_TEMPLATE = "is_template";
		const std::string ScenarioTableSync::COL_NAME = "name";
		const std::string ScenarioTableSync::COL_PERIODSTART = "period_start";
		const std::string ScenarioTableSync::COL_PERIODEND = "period_end"; 

		
		ScenarioTableSync::ScenarioTableSync()
			: SQLiteTableSyncTemplate<Scenario>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_IS_TEMPLATE, "INTEGER");
			addTableColumn(COL_NAME, "TEXT");
			addTableColumn (COL_PERIODSTART, "TIMESTAMP", true);
			addTableColumn (COL_PERIODEND, "TIMESTAMP", true);
		}

		void ScenarioTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
		}

		void ScenarioTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
		}

		void ScenarioTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
		}

		std::vector<Scenario*> ScenarioTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<Scenario*> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					Scenario* object = new Scenario();
					load(object, result, i);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
