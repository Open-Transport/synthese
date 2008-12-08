
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
#include "AlarmObjectLinkException.h"
#include "AlarmRecipient.h"
#include "AlarmObjectLink.h"
#include "SentAlarm.h"
#include "Registry.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,AlarmObjectLinkTableSync>::FACTORY_KEY("99.00.01 Alarm links");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<AlarmObjectLinkTableSync>::TABLE_NAME = "t040_alarm_object_links";
		template<> const int SQLiteTableSyncTemplate<AlarmObjectLinkTableSync>::TABLE_ID = 40;
		template<> const bool SQLiteTableSyncTemplate<AlarmObjectLinkTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>::Load(
			AlarmObjectLink* object,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			object->setAlarmId(rows->getLongLong ( AlarmObjectLinkTableSync::COL_ALARM_ID));
			object->setObjectId(rows->getLongLong ( AlarmObjectLinkTableSync::COL_OBJECT_ID));
			object->setRecipientKey(rows->getText ( AlarmObjectLinkTableSync::COL_RECIPIENT_KEY));
		
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				shared_ptr<AlarmRecipient> ar(Factory<AlarmRecipient>::create(object->getRecipientKey()));
				shared_ptr<SentAlarm> alarm(env->template getEditableRegistry<SentAlarm>().getEditable(object->getAlarmId()));
				try
				{
					ar->addObject(alarm.get(), object->getObjectId());
				}
				catch (AlarmObjectLinkException e)
				{
					Log::GetInstance().error ("Alarm object link error (t040_alarm_object_links table) : ", e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>::Unlink(
			AlarmObjectLink* aol,
			Env* env
		){
			shared_ptr<AlarmRecipient> ar(Factory<AlarmRecipient>::create(aol->getRecipientKey()));
			shared_ptr<SentAlarm> alarm = env->template getEditableRegistry<SentAlarm>().getEditable(aol->getAlarmId());
			ar->removeObject(alarm.get(), aol->getObjectId());
		}


		template<> void SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>::Save(AlarmObjectLink* object)
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
			: SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>()
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
			Env* env(Env::GetOfficialEnv());
			Registry<AlarmObjectLink>& registry(env->template getEditableRegistry<AlarmObjectLink>());
			while (rows->next ())
			{
				shared_ptr<AlarmObjectLink> aol(new AlarmObjectLink);
				Load(aol.get(), rows, env);
				registry.add(aol);
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
				if (!Env::GetOfficialEnv()->template getRegistry<AlarmObjectLink>().contains(rows->getLongLong (TABLE_COL_ID)))
					continue;

				shared_ptr<AlarmObjectLink> aol = Env::GetOfficialEnv()->template getEditableRegistry<AlarmObjectLink>().getEditable(rows->getLongLong (TABLE_COL_ID));
				
				// Alarm not found in ram : this is not a template
				Unlink(aol.get());
				Env::GetOfficialEnv()->template getEditableRegistry<AlarmObjectLink>().remove(rows->getLongLong (TABLE_COL_ID));
			}
		}

		void AlarmObjectLinkTableSync::Remove( uid alarmId, uid objectId )
		{
			std::stringstream query;
			query
				<< "DELETE FROM " << TABLE_NAME
				<< " WHERE " 
				<< COL_ALARM_ID << "=" << Conversion::ToString(alarmId);
			if (objectId != UNKNOWN_VALUE)
				query << " AND " << COL_OBJECT_ID << "=" << Conversion::ToString(objectId);

			DBModule::GetSQLite()->execUpdate(query.str());
		}

		void AlarmObjectLinkTableSync::Search(
			Env& env,
			const Alarm* alarm,
			int first /*= 0*/,
			int number, /*= 0*/
			LinkLevel linkLevel
		){
			std::stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << util::Conversion::ToString(alarm->getKey());
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
