
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
#include "AlarmTemplateInheritedTableSync.h"
#include "Env.h"
#include "ReplaceQuery.h"
#include <sstream>

using namespace std;
using namespace boost;

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
		void DBInheritedTableSyncTemplate<AlarmTableSync,ScenarioSentAlarmInheritedTableSync,SentAlarm>::Load(
			SentAlarm* obj,
			const DBResultSPtr& rows, 
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				obj->setScenario(
					SentScenarioInheritedTableSync::Get(
						rows->getLongLong(AlarmTableSync::COL_SCENARIO_ID),
						env,
						linkLevel
					).get()
				);
				RegistryKeyType id(rows->getLongLong(AlarmTableSync::COL_TEMPLATE_ID));
				if(id > 0)
				{
					obj->setTemplate(
						AlarmTemplateInheritedTableSync::Get(
							id,
							env,
							linkLevel
						).get()
					);
				}
			}
		}



		template<>
		void DBInheritedTableSyncTemplate<
			AlarmTableSync,ScenarioSentAlarmInheritedTableSync, SentAlarm
		>::Unlink(
			SentAlarm* obj
		){
			obj->setScenario(NULL);
		}


		template<>
		void DBInheritedTableSyncTemplate<
			AlarmTableSync,ScenarioSentAlarmInheritedTableSync, SentAlarm
		>::Save(
			SentAlarm* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<AlarmTableSync> query(*object);
			query.addField(0);
			query.addField(0);
			query.addField(object->getLevel());
			query.addField(object->getShortMessage());
			query.addField(object->getLongMessage());
			query.addFieldNull();
			query.addFieldNull();
			query.addField(object->getScenario() ? object->getScenario()->getKey() : RegistryKeyType(0));
			query.addField(object->getTemplate() ? object->getTemplate()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}
	}

	namespace messages
	{
		ScenarioSentAlarmInheritedTableSync::SearchResult ScenarioSentAlarmInheritedTableSync::Search(
			Env& env,
			RegistryKeyType scenarioId,
			int first /*= 0 */,
			boost::optional<std::size_t> number /*= 0 */,
			bool orderByLevel /*= false */,
			bool orderByStatus /*= false */,
			bool orderByConflict /*= false */,
			bool raisingOrder /*= false*/,
			LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE.NAME << " AS a "
				<< " WHERE "
				<< COL_IS_TEMPLATE << "=0"
				<< " AND " << COL_SCENARIO_ID << "=" << scenarioId;
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}

/* voir si c interessant
void SingleSentAlarmInheritedTableSync::Search( util::Env& env, ptime startDate , ptime endDate , AlarmConflict conflict , AlarmLevel level , int first, int number, bool orderByDate, bool orderByLevel,
bool orderByStatus, bool orderByConflict, bool raisingOrder, util::LinkLevel linkLevel )
{
	DB* db = DBModule::GetDB();
	stringstream query;
	query
		<< " SELECT "
		<< "a.*"
		<< ",(SELECT COUNT(" << AlarmObjectLinkTableSync::COL_OBJECT_ID << ") FROM " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol3 WHERE aol3." << AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID << ") AS " << _COL_RECIPIENTS_NUMBER
		<< ",(SELECT MAX(al2."  << COL_LEVEL << ") FROM " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol1 INNER JOIN " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol2 ON aol1." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=aol2." << AlarmObjectLinkTableSync::COL_OBJECT_ID << " AND aol1." << AlarmObjectLinkTableSync::COL_ALARM_ID << " != aol2." << AlarmObjectLinkTableSync::COL_ALARM_ID << " INNER JOIN " << TABLE.NAME << " AS al2 ON al2." << TABLE_COL_ID << " = aol2." << AlarmObjectLinkTableSync::COL_ALARM_ID << " WHERE "
		<< " aol1." << AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID
		<< " AND al2." << COL_IS_TEMPLATE << "=0 "
		<< " AND (al2." << COL_PERIODSTART << " IS NULL OR a." << COL_PERIODEND << " IS NULL OR al2." << COL_PERIODSTART << " <= a." << COL_PERIODEND << ")"
		<< " AND (al2." << COL_PERIODEND << " IS NULL OR a." << COL_PERIODSTART << " IS NULL OR al2." << COL_PERIODEND <<" >= a." << COL_PERIODSTART << ")"
		<< ") AS " << _COL_CONFLICT_LEVEL
		<< " FROM " << TABLE.NAME << " AS a "
		<< " WHERE "
		<< "a." << COL_IS_TEMPLATE << "=0"
		<< " AND " << COL_SCENARIO_ID << "=0";
	if (!startDate.is_not_a_date_time())
		query << " AND a." << COL_PERIODSTART << "<=" << startDate.();
	if (!endDate.is_not_a_date_time())
		query << " AND a." << COL_PERIODEND << ">=" << endDate.();
	if (conflict == ALARM_NO_CONFLICT)
		query << " AND " << _COL_CONFLICT_LEVEL << " IS NULL";
	if (conflict == ALARM_WARNING_ON_INFO)
		query << " AND a." << COL_LEVEL << "=" << ((int) ALARM_LEVEL_WARNING) << " AND " << _COL_CONFLICT_LEVEL << "=" << ((int) ALARM_LEVEL_INFO);
	if (conflict == ALARM_INFO_UNDER_WARNING)
		query << " AND a." << COL_LEVEL << "=" << ((int) ALARM_LEVEL_INFO) << " AND " << _COL_CONFLICT_LEVEL << "=" << ((int) ALARM_LEVEL_WARNING);
	if (conflict == ALARM_CONFLICT)
		query << " AND a." << COL_LEVEL << "=" << _COL_CONFLICT_LEVEL;
	if (level != ALARM_LEVEL_UNKNOWN)
		query << " AND a." << COL_LEVEL << "=" << ((int) level);
	if (number > 0)
		query << " LIMIT " << Conversion::ToString(number + 1);
	if (first > 0)
		query << " OFFSET " << Conversion::ToString(first);

	try
	{
		Registry<SentAlarm>& registry(env.getEditableRegistry<SentAlarm>());
		DBResultSPtr rows = db->execQuery(query.str());
		while (rows->next ())
		{
			shared_ptr<SingleSentAlarm> object(new SingleSentAlarm);
			Load(object.get(), rows, env, linkLevel);
			SingleSentAlarm::Complements c;
			c.recipientsNumber = rows->getInt (_COL_RECIPIENTS_NUMBER);

			if (!rows->getInt (_COL_CONFLICT_LEVEL))
			{
				c.conflictStatus = ALARM_NO_CONFLICT;
			}
			else
			{
				AlarmLevel conflictLevel = static_cast<AlarmLevel>(rows->getInt (_COL_CONFLICT_LEVEL));
				switch (object->getLevel())
				{
				case ALARM_LEVEL_INFO:
					switch (conflictLevel)
					{
					case ALARM_LEVEL_WARNING: c.conflictStatus = ALARM_WARNING_ON_INFO;
					case ALARM_LEVEL_INFO: c.conflictStatus = ALARM_CONFLICT;
					}
					break;

				case ALARM_LEVEL_WARNING:
					switch (conflictLevel)
					{
					case ALARM_LEVEL_WARNING: c.conflictStatus = ALARM_CONFLICT;
					case ALARM_LEVEL_INFO: c.conflictStatus = ALARM_INFO_UNDER_WARNING;
					}
				}
			}
			object->setComplements(c);
			registry.add(static_pointer_cast<SentAlarm,SingleSentAlarm>(object));
		}
	}
	catch(DBException& e)
	{
		throw Exception(e.getMessage());
	}

}
*/