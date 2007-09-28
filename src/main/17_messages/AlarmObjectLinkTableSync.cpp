
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

#include "AlarmObjectLinkTableSync.h"

#include "17_messages/AlarmRecipient.h"
#include "17_messages/AlarmObjectLink.h"
#include "17_messages/SentAlarm.h"

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

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink>::load(AlarmObjectLink* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setAlarmId(rows->getLongLong ( AlarmObjectLinkTableSync::COL_ALARM_ID));
			object->setObjectId(rows->getLongLong ( AlarmObjectLinkTableSync::COL_OBJECT_ID));
			object->setRecipientKey(rows->getText ( AlarmObjectLinkTableSync::COL_RECIPIENT_KEY));
		}

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink>::save(AlarmObjectLink* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
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

		void AlarmObjectLinkTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				AlarmObjectLink* aol(new AlarmObjectLink);
				load(aol, rows);
				
				// Alarm not found in ram : this is a template
				if (!SentAlarm::Contains(aol->getAlarmId()))
					continue;

				shared_ptr<AlarmRecipient> ar = Factory<AlarmRecipient>::createSharedPtr(aol->getRecipientKey());
				shared_ptr<SentAlarm> alarm(SentAlarm::GetUpdateable(aol->getAlarmId()));
				ar->addObject(alarm.get(), aol->getObjectId());
				aol->store();
			}
		}

		void AlarmObjectLinkTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			rowsAdded(sqlite, sync, rows);
		}

		void AlarmObjectLinkTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				if (!AlarmObjectLink::Contains(rows->getLongLong (TABLE_COL_ID)))
					continue;

				shared_ptr<AlarmObjectLink> aol = AlarmObjectLink::GetUpdateable(rows->getLongLong (TABLE_COL_ID));
				
				// Alarm not found in ram : this is a template
				if (SentAlarm::Contains(aol->getAlarmId()))
				{
					shared_ptr<AlarmRecipient> ar = Factory<AlarmRecipient>::createSharedPtr(aol->getRecipientKey());
					shared_ptr<SentAlarm> alarm = SentAlarm::GetUpdateable(aol->getAlarmId());
					ar->removeObject(alarm.get(), aol->getObjectId());
				}
				AlarmObjectLink::Remove(rows->getLongLong (TABLE_COL_ID));
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
				db::SQLiteResultSPtr rows = db::DBModule::GetSQLite()->execQuery(query.str());
				std::vector< boost::shared_ptr<AlarmObjectLink> > objects;
				while (rows->next ())
				{
					shared_ptr<AlarmObjectLink> object(new AlarmObjectLink);
					load(object.get(), rows);
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
