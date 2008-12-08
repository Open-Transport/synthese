
/** SingleSentAlarmInheritedTableSync class implementation.
	@file SingleSentAlarmInheritedTableSync.cpp

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

#include "SingleSentAlarmInheritedTableSync.h"
#include "DateTime.h"
#include "AlarmObjectLinkTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;
	using namespace time;
	

	template<>
	const string util::FactorableTemplate<AlarmTableSync, SingleSentAlarmInheritedTableSync>::FACTORY_KEY("SingleSentAlarmInheritedTableSync");

	namespace db
	{

		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,SingleSentAlarmInheritedTableSync,SingleSentAlarm>::Load(
			SingleSentAlarm* obj,
			const SQLiteResultSPtr& rows, 
			Env* env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);
			obj->setPeriodStart(DateTime::FromSQLTimestamp (rows->getText ( AlarmTableSync::COL_PERIODSTART)));
			obj->setPeriodEnd(DateTime::FromSQLTimestamp (rows->getText ( AlarmTableSync::COL_PERIODEND)));
			obj->setIsEnabled(rows->getBool ( AlarmTableSync::COL_ENABLED));
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,SingleSentAlarmInheritedTableSync,SingleSentAlarm>::Unlink(SingleSentAlarm* obj, Env* env)
		{
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,SingleSentAlarmInheritedTableSync, SingleSentAlarm>::Save(SingleSentAlarm* obj)
		{
			if (obj->getKey() == UNKNOWN_VALUE)
				obj->setKey(getId());

			stringstream query;
			query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(obj->getKey())
				<< ",1"
				<< ",0"
				<< "," << Conversion::ToString(static_cast<int>(obj->getLevel()))
				<< "," << Conversion::ToSQLiteString(obj->getShortMessage())
				<< "," << Conversion::ToSQLiteString(obj->getLongMessage())
				<< ",''"
				<< ",''"
				<< ",O"
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}

	namespace messages
	{

		SingleSentAlarmInheritedTableSync::SingleSentAlarmInheritedTableSync()
			: SQLiteInheritedRegistryTableSync<AlarmTableSync, SingleSentAlarmInheritedTableSync, SingleSentAlarm>()
		{

		}



		void SingleSentAlarmInheritedTableSync::Search( util::Env& env, time::DateTime startDate , time::DateTime endDate , AlarmConflict conflict , AlarmLevel level , int first /*= 0 */, int number /*= 0 */, bool orderByDate /*= true */, bool orderByLevel /*= false */, bool orderByStatus /*= false */, bool orderByConflict /*= false */, bool raisingOrder /*= false*/, util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT "
				<< "a.*"
				<< ",(SELECT COUNT(" << AlarmObjectLinkTableSync::COL_OBJECT_ID << ") FROM " << AlarmObjectLinkTableSync::TABLE_NAME << " AS aol3 WHERE aol3." << AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID << ") AS " << _COL_RECIPIENTS_NUMBER
				<< ",(SELECT MAX(al2."  << COL_LEVEL << ") FROM " << AlarmObjectLinkTableSync::TABLE_NAME << " AS aol1 INNER JOIN " << AlarmObjectLinkTableSync::TABLE_NAME << " AS aol2 ON aol1." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=aol2." << AlarmObjectLinkTableSync::COL_OBJECT_ID << " AND aol1." << AlarmObjectLinkTableSync::COL_ALARM_ID << " != aol2." << AlarmObjectLinkTableSync::COL_ALARM_ID << " INNER JOIN " << TABLE_NAME << " AS al2 ON al2." << TABLE_COL_ID << " = aol2." << AlarmObjectLinkTableSync::COL_ALARM_ID << " WHERE "
				<< " aol1." << AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID
				<< " AND al2." << COL_IS_TEMPLATE << "=0 "
				<< " AND (al2." << COL_PERIODSTART << " IS NULL OR a." << COL_PERIODEND << " IS NULL OR al2." << COL_PERIODSTART << " <= a." << COL_PERIODEND << ")"
				<< " AND (al2." << COL_PERIODEND << " IS NULL OR a." << COL_PERIODSTART << " IS NULL OR al2." << COL_PERIODEND <<" >= a." << COL_PERIODSTART << ")"
				<< ") AS " << _COL_CONFLICT_LEVEL
				<< " FROM " << TABLE_NAME << " AS a "
				<< " WHERE "
				<< "a." << COL_IS_TEMPLATE << "=0"
				<< " AND " << COL_SCENARIO_ID << "=0";
			if (!startDate.isUnknown())
				query << " AND a." << COL_PERIODSTART << "<=" << startDate.toSQLString();
			if (!endDate.isUnknown())
				query << " AND a." << COL_PERIODEND << ">=" << endDate.toSQLString();
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
				Registry<SingleSentAlarm>& registry(env.template getEditableRegistry<SingleSentAlarm>());
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				while (rows->next ())
				{
					shared_ptr<SingleSentAlarm> object(new SingleSentAlarm);
					Load(object.get(), rows, &env, linkLevel);
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
					registry.add(object);
				}
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}

		}
	}
}