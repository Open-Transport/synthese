
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

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate< AlarmObjectLink<Registrable<uid, void> > >::TABLE_NAME = "t040_alarm_object_links";
		template<> const int SQLiteTableSyncTemplate< AlarmObjectLink<Registrable<uid, void> > >::TABLE_ID = 040;
		template<> const bool SQLiteTableSyncTemplate< AlarmObjectLink<Registrable<uid, void> > >::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink<Registrable<uid, void> > >::load(AlarmObjectLink<Registrable<uid, void> >* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
		}

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink<Registrable<uid, void> > >::save(AlarmObjectLink<Registrable<uid, void> >* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId(1,1));
            query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getRecipientKey())
				<< "," << Conversion::ToString(object->getObjectId())
				<< "," << Conversion::ToString(object->getAlarm()->getKey())
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
			: SQLiteTableSyncTemplate<AlarmObjectLink<Registrable<uid, void> > >(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_RECIPIENT_KEY, "TEXT");
			addTableColumn(COL_OBJECT_ID, "INTEGER");
			addTableColumn(COL_ALARM_ID, "INTEGER");			
		}

		void AlarmObjectLinkTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				AlarmRecipient* ar = Factory<AlarmRecipient>::create(rows.getColumn(i, COL_RECIPIENT_KEY));
				Alarm* alarm = MessagesModule::getAlarms().get(Conversion::ToLongLong(rows.getColumn(i, COL_ALARM_ID)));
				ar->addObject(alarm, Conversion::ToLongLong(rows.getColumn(i, COL_OBJECT_ID)));
			}
		}

		void AlarmObjectLinkTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			rowsAdded(sqlite, sync, rows);
		}

		void AlarmObjectLinkTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				AlarmRecipient* ar = Factory<AlarmRecipient>::create(rows.getColumn(i, COL_RECIPIENT_KEY));
				Alarm* alarm = MessagesModule::getAlarms().get(Conversion::ToLongLong(rows.getColumn(i, COL_ALARM_ID)));
				ar->removeObject(alarm, Conversion::ToLongLong(rows.getColumn(i, COL_OBJECT_ID)));
			}
		}
	}
}
