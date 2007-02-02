
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

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate< AlarmObjectLink<void> >::TABLE_NAME = "t040_alarm_object_links";
		template<> const int SQLiteTableSyncTemplate< AlarmObjectLink<void> >::TABLE_ID = 040;
		template<> const bool SQLiteTableSyncTemplate< AlarmObjectLink<void> >::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink<void> >::load(AlarmObjectLink<void>* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
		}

		template<> void SQLiteTableSyncTemplate< AlarmObjectLink<void> >::save(AlarmObjectLink<void>* object)
		{
			Registrable<uid, void>* obj = (Registrable<uid, void>*) object->getObject();
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					<< AlarmObjectLinkTableSync::COL_RECIPIENT_KEY << "=" << Conversion::ToSQLiteString(object->getRecipientKey())
					<< "," << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=" << Conversion::ToString(obj->getKey())
					<< "," << AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << Conversion::ToString(object->getAlarm()->getKey())
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId(1,1));
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					<< "," << Conversion::ToSQLiteString(object->getRecipientKey())
					<< "," << Conversion::ToString(obj->getKey())
					<< "," << Conversion::ToString(object->getAlarm()->getKey())
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace messages
	{
		const std::string AlarmObjectLinkTableSync::COL_RECIPIENT_KEY = "recipient_key";
		const std::string AlarmObjectLinkTableSync::COL_OBJECT_ID = "object_id";
		const std::string AlarmObjectLinkTableSync::COL_ALARM_ID = "alarm_id";


		AlarmObjectLinkTableSync::AlarmObjectLinkTableSync()
			: SQLiteTableSyncTemplate<AlarmObjectLink<void> >(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_RECIPIENT_KEY, "TEXT");
			addTableColumn(COL_OBJECT_ID, "INTEGER");
			addTableColumn(COL_ALARM_ID, "INTEGER");			
		}

		void AlarmObjectLinkTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			/// @todo Implementation
			for (int i=0; i<rows.getNbRows(); ++i)
			{
//				AlarmObjectLink<void>* object = new AlarmObjectLink<void>();
//				load(object, rows, i);
				/// @todo Add the object to the corresponding register
				// Eg : Module::getObjects().add(object);
			}
		}

		void AlarmObjectLinkTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
//			for (int i=0; i<rows.getNbRows(); ++i)
//			{
				/// @todo search and update corresponding objects
//			}
		}

		void AlarmObjectLinkTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
//			for (int i=0; i<rows.getNbRows(); ++i)
//			{
				/// @todo search and destroy corresponding objects
//			}
		}

	}
}
