
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

#include "17_messages/ScenarioTableSync.h"
#include "17_messages/SentScenario.h"
#include "17_messages/SentScenarioInheritedTableSync.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTemplateInheritedTableSync.h"
#include "17_messages/AlarmTableSync.h"

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "04_time/DateTime.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,ScenarioTableSync>::FACTORY_KEY("17.00.01 Alarm scenarii");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ScenarioTableSync>::TABLE_NAME = "t039_scenarios";
		template<> const int SQLiteTableSyncTemplate<ScenarioTableSync>::TABLE_ID = 39;
		template<> const bool SQLiteTableSyncTemplate<ScenarioTableSync>::HAS_AUTO_INCREMENT = true;


		template<>
		string SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::_GetSubClassKey(const SQLiteResultSPtr& row)
		{
			return row->getBool(ScenarioTableSync::COL_IS_TEMPLATE)
				? ScenarioTemplateInheritedTableSync::FACTORY_KEY
				: SentScenarioInheritedTableSync::FACTORY_KEY
				;
		}


		template<> void SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::Load(Scenario* object, const db::SQLiteResultSPtr& rows )
		{
			object->setName(rows->getText ( ScenarioTableSync::COL_NAME));
			if (rows->getBool ( ScenarioTableSync::COL_IS_TEMPLATE))
			{
				ScenarioTemplate* tobject = static_cast<ScenarioTemplate*>(object);
				tobject->setKey(rows->getLongLong (TABLE_COL_ID));
				tobject->setFolderId(rows->getLongLong(ScenarioTableSync::COL_FOLDER_ID));
			}
			else
			{
				SentScenario* sobject = static_cast<SentScenario*>(object);
				sobject->setIsEnabled(rows->getBool ( ScenarioTableSync::COL_ENABLED));
				sobject->setPeriodStart(DateTime::FromSQLTimestamp (rows->getText ( ScenarioTableSync::COL_PERIODSTART)));
				sobject->setPeriodEnd(DateTime::FromSQLTimestamp (rows->getText ( ScenarioTableSync::COL_PERIODEND)));
				sobject->setKey(rows->getLongLong (TABLE_COL_ID));
			}
		}

		template<> void SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::Save(Scenario* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			
			ScenarioTemplate* tobject = dynamic_cast<ScenarioTemplate*>(object);
			if (tobject)
			{
				if (tobject->getKey() <= 0)
					tobject->setKey(getId());
			}
			SentScenario* sobject = dynamic_cast<SentScenario*>(object);
			if (sobject)
			{
				if (sobject->getKey() <= 0)
					sobject->setKey(getId());
			}

            query
				<< "REPLACE INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getId())
					<< "," << Conversion::ToString(tobject != NULL)
					<< "," << (sobject ? Conversion::ToString(sobject->getIsEnabled()) : "0")
					<< "," << Conversion::ToSQLiteString(object->getName())
					<< "," << (sobject ? sobject->getPeriodStart().toSQLString() : "NULL")
					<< "," << (sobject ? sobject->getPeriodEnd().toSQLString() : "NULL")
					<< "," << Conversion::ToString(tobject ? tobject->getFolderId() : UNKNOWN_VALUE)
				<< ")";
			sqlite->execUpdate(query.str());

			if (sobject)
			{
				stringstream alarmquery;
				alarmquery
					<< "UPDATE "
						<< AlarmTableSync::TABLE_NAME
					<< " SET "
						<< AlarmTableSync::COL_PERIODSTART << "=" << sobject->getPeriodStart().toSQLString()
						<< "," << AlarmTableSync::COL_PERIODEND << "=" << sobject->getPeriodEnd().toSQLString()
					<< " WHERE " 
						<< AlarmTableSync::COL_SCENARIO_ID << "=" << sobject->getKey();
				sqlite->execUpdate(alarmquery.str());
			}

		}
	}

	namespace messages
	{
		const std::string ScenarioTableSync::COL_IS_TEMPLATE = "is_template";
		const std::string ScenarioTableSync::COL_ENABLED = "is_enabled";
		const std::string ScenarioTableSync::COL_NAME = "name";
		const std::string ScenarioTableSync::COL_PERIODSTART = "period_start";
		const std::string ScenarioTableSync::COL_PERIODEND = "period_end"; 
		const std::string ScenarioTableSync::COL_FOLDER_ID("folder_id");

		
		ScenarioTableSync::ScenarioTableSync()
			: SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_IS_TEMPLATE, "INTEGER");
			addTableColumn(COL_ENABLED, "INTEGER");
			addTableColumn(COL_NAME, "TEXT");
			addTableColumn (COL_PERIODSTART, "TIMESTAMP", true);
			addTableColumn (COL_PERIODEND, "TIMESTAMP", true);
			addTableColumn(COL_FOLDER_ID, "INTEGER");

			vector<string> cols;
			cols.push_back(COL_IS_TEMPLATE);
			cols.push_back(COL_PERIODSTART);
			addTableIndex(cols);

			addTableIndex(COL_FOLDER_ID);
		}

/*		void ScenarioTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    while (rows->next ())
			{
			    if (rows->getBool (COL_IS_TEMPLATE))
				continue;
				
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (SentScenario::Contains (id))
				{
					shared_ptr<SentScenario> scenario(SentScenario::GetUpdateable(id));
					Load(scenario.get(), rows);
				}
				else
				{
					SentScenario* scenario(new SentScenario);
					Load(scenario, rows);
					scenario->store();
				}
			}
		}

		void ScenarioTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
		    while (rows->next ())
			{
			    if (rows->getBool (COL_IS_TEMPLATE))
					continue;

				uid id = rows->getLongLong (TABLE_COL_ID);
				shared_ptr<SentScenario> alarm = SentScenario::GetUpdateable(id);
				Load(alarm.get(), rows);
			}
		}

		void ScenarioTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
		    while (rows->next ())
		    {
			if (rows->getBool (COL_IS_TEMPLATE))
					continue;

			    uid id = rows->getLongLong (TABLE_COL_ID);
			    SentScenario::Remove (id);	/// @todo Not so simple.
			}
		}
*/
		std::vector<shared_ptr<SentScenario> > ScenarioTableSync::searchSent(
			time::DateTime startDate
			, time::DateTime endDate
			, const std::string name
			, int first /*= 0*/, int number /*= 0*/
			, bool orderByDate
			, bool orderByName
			, bool orderByStatus
			, bool orderByConflict
			, bool raisingOrder
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
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

			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<SentScenario> > objects;
				while (rows->next ())
				{
					shared_ptr<SentScenario> object(new SentScenario);
					Load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		std::vector<boost::shared_ptr<ScenarioTemplate> > ScenarioTableSync::searchTemplate(
			uid folderId
			, const std::string name /*= std::string() */
			, const ScenarioTemplate* scenarioToBeDifferentWith
			, int first /*= 0 */
			, int number /*= -1 */
			, bool orderByName /*= true */
			, bool raisingOrder /*= false */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				<< COL_IS_TEMPLATE << "=1";
			if (folderId > 0)
				query << " AND " << COL_FOLDER_ID << "=" << Conversion::ToString(folderId);
			else if (folderId == 0)
				query << " AND (" << COL_FOLDER_ID << "=0 OR " << COL_FOLDER_ID << " IS NULL)";
				
			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToSQLiteString(name);
			if (scenarioToBeDifferentWith)
				query << " AND " << TABLE_COL_ID << "!=" << scenarioToBeDifferentWith->getKey();
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<ScenarioTemplate> > objects;
				while (rows->next ())
				{
					shared_ptr<ScenarioTemplate> object(new ScenarioTemplate(rows->getText (COL_NAME)));
					Load (object.get(), rows);
					objects.push_back (object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}

		}


/**		boost::shared_ptr<Scenario> ScenarioTableSync::getScenario(uid key)
		{
			std::stringstream query;
			query
				<< "SELECT * "
				<< "FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(key)
				<< " LIMIT 1";

			db::SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
			if (rows->next () == false)
			    throw DBEmptyResultException(key);

			boost::shared_ptr<Scenario> object(
			    rows->getBool (ScenarioTableSync::COL_IS_TEMPLATE)
			    ? static_cast<Scenario*>(new ScenarioTemplate(rows->getText (ScenarioTableSync::COL_NAME)))
			    : static_cast<Scenario*>(new SentScenario)
			    );

			load(object.get(), rows);
			return object;
		}
*/

	}
}
