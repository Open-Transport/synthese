
/** AlarmTableSync class implementation.
	@file AlarmTableSync.cpp

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

#include "17_messages/AlarmTableSync.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/ScenarioSentAlarm.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/Types.h"
#include "17_messages/MessagesModule.h"

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "04_time/DateTime.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace time;
	using namespace util;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Alarm>::TABLE_NAME = "t003_alarms";
		template<> const int SQLiteTableSyncTemplate<Alarm>::TABLE_ID = 3;
		template<> const bool SQLiteTableSyncTemplate<Alarm>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Alarm>::load(Alarm* alarm, const SQLiteResult& rows, int rowId)
		{
			if (Conversion::ToBool(rows.getColumn(rowId, AlarmTableSync::COL_IS_TEMPLATE)))
			{
				AlarmTemplate* talarm = dynamic_cast<AlarmTemplate*>(alarm);
				talarm->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			}
			else
			{
				if (Conversion::ToLongLong(rows.getColumn (rowId, AlarmTableSync::COL_SCENARIO_ID)))
				{
					ScenarioSentAlarm* salarm = dynamic_cast<ScenarioSentAlarm*>(alarm);
					salarm->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
				}
				else
				{
					SingleSentAlarm* salarm = dynamic_cast<SingleSentAlarm*>(alarm);
					salarm->setPeriodStart(DateTime::FromSQLTimestamp (rows.getColumn (rowId, AlarmTableSync::COL_PERIODSTART)));
					salarm->setPeriodEnd(DateTime::FromSQLTimestamp (rows.getColumn (rowId, AlarmTableSync::COL_PERIODEND)));
					salarm->setIsEnabled(Conversion::ToBool(rows.getColumn(rowId, AlarmTableSync::COL_ENABLED)));
					salarm->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
				}
			}
			alarm->setLevel((AlarmLevel) Conversion::ToInt (rows.getColumn (rowId, AlarmTableSync::COL_LEVEL)));
			alarm->setShortMessage(rows.getColumn (rowId, AlarmTableSync::COL_SHORT_MESSAGE));
			alarm->setLongMessage(rows.getColumn (rowId, AlarmTableSync::COL_LONG_MESSAGE));
		}

		template<> void SQLiteTableSyncTemplate<Alarm>::save(Alarm* object)
		{
			stringstream query;

			ScenarioSentAlarm* sobject = dynamic_cast<ScenarioSentAlarm*>(object);
			AlarmTemplate* tobject = dynamic_cast<AlarmTemplate*>(object);
			SingleSentAlarm* gobject = dynamic_cast<SingleSentAlarm*>(object);

			if (sobject)
			{
				if (!sobject->getKey())
					sobject->setKey(getId(1,1));
				query
					<< " REPLACE INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(sobject->getKey())
					<< ",O"
					<< "," << Conversion::ToString(sobject->getIsEnabled())
					<< "," << Conversion::ToString((int) sobject->getLevel())
					<< "," << Conversion::ToSQLiteString(sobject->getShortMessage())
					<< "," << Conversion::ToSQLiteString(sobject->getLongMessage())
					<< "," << sobject->getPeriodStart().toSQLString()
					<< "," << sobject->getPeriodEnd().toSQLString()
					<< "," << Conversion::ToString(sobject->getScenario().getKey())
					<< ")";
			}
			else if (gobject)
			{
				if (!gobject->getKey())
					gobject->setKey(getId(1,1));
				query
					<< " REPLACE INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(gobject->getKey())
					<< ",O"
					<< "," << Conversion::ToString(gobject->getIsEnabled())
					<< "," << Conversion::ToString((int) gobject->getLevel())
					<< "," << Conversion::ToSQLiteString(gobject->getShortMessage())
					<< "," << Conversion::ToSQLiteString(gobject->getLongMessage())
					<< "," << gobject->getPeriodStart().toSQLString()
					<< "," << gobject->getPeriodEnd().toSQLString()
					<< ",0"
					<< ")";
			}
			else if (tobject)
			{
				if (!tobject->getKey())
					tobject->setKey(getId(1,1));
			
				query
					<< " REPLACE INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(tobject->getKey())
					<< ",1"
					<< ",0"
					<< "," << Conversion::ToString((int) tobject->getLevel())
					<< "," << Conversion::ToSQLiteString(tobject->getShortMessage())
					<< "," << Conversion::ToSQLiteString(tobject->getLongMessage())
					<< ",''"
					<< ",''"
					<< "," << Conversion::ToString(tobject->getScenarioId())
					<< ")";
			}
			DBModule::GetSQLite()->execUpdate(query.str());
		}

	}


	namespace messages
	{
		const std::string AlarmTableSync::COL_IS_TEMPLATE = "is_template";
		const std::string AlarmTableSync::COL_ENABLED = "is_enabled";
		const std::string AlarmTableSync::COL_LEVEL = "level";
		const std::string AlarmTableSync::COL_SHORT_MESSAGE = "short_message"; 
		const std::string AlarmTableSync::COL_LONG_MESSAGE = "long_message";
		const std::string AlarmTableSync::COL_PERIODSTART = "period_start";
		const std::string AlarmTableSync::COL_PERIODEND = "period_end"; 
		const std::string AlarmTableSync::COL_SCENARIO_ID = "scenario_id"; 
		
		AlarmTableSync::AlarmTableSync ()
		: SQLiteTableSyncTemplate<Alarm>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_IS_TEMPLATE, "INTEGER", true);
			addTableColumn(COL_ENABLED, "INTEGER");
			addTableColumn (COL_LEVEL, "INTEGER", true);
			addTableColumn (COL_SHORT_MESSAGE, "TEXT", true);
			addTableColumn (COL_LONG_MESSAGE, "TEXT", true);
			addTableColumn (COL_PERIODSTART, "TIMESTAMP", true);
			addTableColumn (COL_PERIODEND, "TIMESTAMP", true);
			addTableColumn(COL_SCENARIO_ID, "INTEGER");

			vector<string> cols;
			cols.push_back(COL_SCENARIO_ID);
			cols.push_back(COL_PERIODSTART);
			addTableIndex(cols);
		}


		AlarmTableSync::~AlarmTableSync ()
		{

		}
		    

		void AlarmTableSync::rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows(); ++rowIndex)
			{
				if (Conversion::ToBool(rows.getColumn(rowIndex, COL_IS_TEMPLATE)))
					continue;

				shared_ptr<SentAlarm> alarm;
				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
				if (MessagesModule::getAlarms ().contains (id))
				{
					alarm = MessagesModule::getAlarms().getUpdateable(id);
					load(alarm.get(), rows, rowIndex);
				}
				else
				{
					uid scenarioId = Conversion::ToLongLong(rows.getColumn(rowIndex, COL_SCENARIO_ID));
					if (scenarioId)
					{
						if (!MessagesModule::getScenarii().contains(scenarioId))
							continue;

						shared_ptr<SentScenario> scenario = MessagesModule::getScenarii().getUpdateable(scenarioId);
						ScenarioSentAlarm* salarm = new ScenarioSentAlarm(*scenario);
						alarm.reset(salarm);
						scenario->addAlarm(salarm);
					}
					else
					{
						alarm.reset(new SingleSentAlarm);
					}
					load(alarm.get(), rows, rowIndex);
					MessagesModule::getAlarms().add (alarm);
				}
			}
		}


		void AlarmTableSync::rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows(); ++rowIndex)
			{
				if (Conversion::ToBool(rows.getColumn(rowIndex, COL_IS_TEMPLATE)))
					continue;

				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
				shared_ptr<SentAlarm> alarm = MessagesModule::getAlarms ().getUpdateable(id);
				load(alarm.get(), rows, rowIndex);
			}
		}


		void AlarmTableSync::rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows(); ++rowIndex)
			{
				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
				if (MessagesModule::getAlarms().contains(id))
				{
					shared_ptr<SentAlarm> alarm = MessagesModule::getAlarms().getUpdateable(id);
					shared_ptr<ScenarioSentAlarm> salarm = dynamic_pointer_cast<ScenarioSentAlarm, SentAlarm>(alarm);
					if (salarm.get())
					{
						MessagesModule::getScenarii().getUpdateable(salarm->getScenario().getKey())->removeAlarm(salarm.get());
					}
					MessagesModule::getAlarms ().remove (id);
				}
			}
		}

		std::vector<shared_ptr<SingleSentAlarm> > AlarmTableSync::searchSingleSent(
			time::DateTime startDate
			, time::DateTime endDate
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByDate
			, bool orderByLevel
			, bool orderByStatus
			, bool orderByConflict
			, bool raisingOrder
		){
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE_NAME << " AS a "
				<< " WHERE "
				<< COL_IS_TEMPLATE << "=0";
			if (!startDate.isUnknown())
				query << " AND " << COL_PERIODSTART << "<=" << startDate.toSQLString();
			if (!endDate.isUnknown())
				query << " AND " << COL_PERIODEND << ">=" << endDate.toSQLString();
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<SingleSentAlarm> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<SingleSentAlarm> object(new SingleSentAlarm);
					load(object.get(), result, i);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		shared_ptr<Alarm> AlarmTableSync::getAlarm(uid key)
		{
			std::stringstream query;
			query
				<< "SELECT * "
				<< "FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(key)
				<< " LIMIT 1";
			db::SQLiteResult rows = DBModule::GetSQLite()->execQuery(query.str());
			if (rows.getNbRows() <= 0)
				throw DBEmptyResultException<Alarm>(key, "ID not found in database.");

			boost::shared_ptr<Alarm> object;
			uid scenarioId = Conversion::ToLongLong(rows.getColumn (0, AlarmTableSync::COL_SCENARIO_ID));
			if (Conversion::ToBool(rows.getColumn(0, AlarmTableSync::COL_IS_TEMPLATE)))
			{
				object.reset(new AlarmTemplate(scenarioId));
			}
			else
			{
				if (scenarioId)
				{
					if (!MessagesModule::getScenarii().contains(scenarioId))
						throw DBEmptyResultException<Alarm>(key, "No such scenario in RAM");

					shared_ptr<const SentScenario> scenario = MessagesModule::getScenarii().get(scenarioId);
					object.reset(new ScenarioSentAlarm(*scenario));
				}
				else
				{
					object.reset(new SingleSentAlarm);
				}
			}
			load(object.get(), rows);
			return object;
		}

		std::vector<boost::shared_ptr<Alarm> > AlarmTableSync::search( const Scenario* scenario , int first /*= 0 */, int number /*= -1 */, bool orderByLevel /*= false */, bool raisingOrder /*= false */ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE_NAME << " AS a "
				<< " WHERE "
				<< COL_SCENARIO_ID << "=" << scenario->getId();
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<Alarm> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<Alarm> object(
						Conversion::ToBool(result.getColumn(i, COL_IS_TEMPLATE))
						? static_cast<Alarm*>(new AlarmTemplate(Conversion::ToLongLong(result.getColumn(i, COL_SCENARIO_ID))))
						: static_cast<Alarm*>(new SingleSentAlarm));
					load(object.get(), result, i);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}

		}

		std::vector<boost::shared_ptr<ScenarioSentAlarm> > AlarmTableSync::searchScenarioSent( const SentScenario* scenario , int first /*= 0 */, int number /*= 0 */, bool orderByLevel /*= false */, bool orderByStatus /*= false */, bool orderByConflict /*= false */, bool raisingOrder /*= false */ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
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

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<ScenarioSentAlarm> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<ScenarioSentAlarm> object(new ScenarioSentAlarm(*scenario));
					load(object.get(), result, i);
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
