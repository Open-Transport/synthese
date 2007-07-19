
/** AlarmObjectLinkTableSync class implementation.
	@file AlarmObjectLinkTableSync.cpp

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

#include "17_messages/AlarmObjectLinkTableSync.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/AlarmObjectLink.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate< AlarmObjectLink>::TABLE_NAME = "t040_alarm_object_links";
		template<> const int SQLiteTableSyncTemplate< AlarmObjectLink>::TABLE_ID = 40;
		template<> const bool SQLiteTableSyncTemplate< AlarmObjectLink>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink>::load(AlarmObjectLink* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setAlarmId(Conversion::ToLongLong(rows.getColumn(rowId, AlarmObjectLinkTableSync::COL_ALARM_ID)));
			object->setObjectId(Conversion::ToLongLong(rows.getColumn(rowId, AlarmObjectLinkTableSync::COL_OBJECT_ID)));
			object->setRecipientKey(rows.getColumn(rowId, AlarmObjectLinkTableSync::COL_RECIPIENT_KEY));
		}

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink>::save(AlarmObjectLink* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
            query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getRecipientKey())
				<< "," << Conversion::ToString(object->getObjectId())
				<< "," << Conversion::ToString(object->getAlarmId())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace messages
	{
		const std::string AlarmObjectLinkTableSync::COL_RECIPIENT_KEY = "recipient_key";
		const std::string AlarmObjectLinkTableSync::COL_OBJECT_ID = "object_id";
		const std::string AlarmObjectLinkTableSync::COL_ALARM_ID = "alarm_id";


		AlarmObjectLinkTableSync::AlarmObjectLinkTableSync()
			: SQLiteTableSyncTemplate<AlarmObjectLink>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_RECIPIENT_KEY, "TEXT");
			addTableColumn(COL_OBJECT_ID, "INTEGER");
			addTableColumn(COL_ALARM_ID, "INTEGER");

			vector<string> c;
			c.push_back(COL_OBJECT_ID);
			c.push_back(COL_ALARM_ID);
			addTableIndex(c);
			addTableIndex(COL_ALARM_ID);
		}

		void AlarmObjectLinkTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				shared_ptr<AlarmObjectLink> aol(new AlarmObjectLink);
				load(aol.get(), rows, i);
				
				// Alarm not found in ram : this is a template
				if (!MessagesModule::getAlarms().contains(aol->getAlarmId()))
					continue;

				shared_ptr<AlarmRecipient> ar = Factory<AlarmRecipient>::create(aol->getRecipientKey());
				shared_ptr<SentAlarm> alarm = MessagesModule::getAlarms().getUpdateable(aol->getAlarmId());
				ar->addObject(alarm.get(), aol->getObjectId());
				MessagesModule::getAlarmLinks().add(aol);
			}
		}

		void AlarmObjectLinkTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			rowsAdded(sqlite, sync, rows);
		}

		void AlarmObjectLinkTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (!MessagesModule::getAlarmLinks().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
					continue;

				shared_ptr<AlarmObjectLink> aol = MessagesModule::getAlarmLinks().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
				
				// Alarm not found in ram : this is a template
				if (MessagesModule::getAlarms().contains(aol->getAlarmId()))
				{
					shared_ptr<AlarmRecipient> ar = Factory<AlarmRecipient>::create(aol->getRecipientKey());
					shared_ptr<SentAlarm> alarm = MessagesModule::getAlarms().getUpdateable(aol->getAlarmId());
					ar->removeObject(alarm.get(), aol->getObjectId());
				}
				MessagesModule::getAlarmLinks().remove(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
			}
		}

		void AlarmObjectLinkTableSync::remove( uid alarmId, uid objectId )
		{
			std::stringstream query;
			query
				<< "DELETE FROM " << TABLE_NAME
				<< " WHERE " 
				<< COL_ALARM_ID << "=" << Conversion::ToString(alarmId)
				<< " AND " << COL_OBJECT_ID << "=" << Conversion::ToString(objectId)
				;
			DBModule::GetSQLite()->execUpdate(query.str());
		}

		std::vector<boost::shared_ptr<AlarmObjectLink> > AlarmObjectLinkTableSync::search( const Alarm* alarm, int first /*= 0*/, int number /*= 0*/ )
		{
			std::stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << util::Conversion::ToString(alarm->getId());
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				db::SQLiteResult result = db::DBModule::GetSQLite()->execQuery(query.str());
				std::vector< boost::shared_ptr<AlarmObjectLink> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<AlarmObjectLink> object(new AlarmObjectLink);
					load(object.get(), result, i);
					objects.push_back(object);
				}
				return objects;
			}
			catch(db::SQLiteException& e)
			{
				throw util::Exception(e.getMessage());
			}
		}
	}
}
