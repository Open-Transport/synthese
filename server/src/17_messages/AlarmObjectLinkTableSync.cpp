
/** AlarmObjectLinkTableSync class implementation.
	@file AlarmObjectLinkTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "SentScenario.h"
#include "AlarmObjectLinkException.h"
#include "AlarmRecipient.h"
#include "AlarmObjectLink.h"
#include "Profile.h"
#include "Alarm.h"
#include "Session.h"
#include "User.h"
#include "Registry.h"
#include "AlarmTableSync.h"
#include "LoadException.h"
#include "LinkException.h"
#include "DeleteQuery.hpp"
#include "ReplaceQuery.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,AlarmObjectLinkTableSync>::FACTORY_KEY("99.00.01 Alarm links");
	}

	namespace messages
	{
		const string AlarmObjectLinkTableSync::COL_RECIPIENT_KEY("recipient_key");
		const string AlarmObjectLinkTableSync::COL_OBJECT_ID("object_id");
		const string AlarmObjectLinkTableSync::COL_ALARM_ID("alarm_id");
		const string AlarmObjectLinkTableSync::COL_PARAMETER = "parameter";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<AlarmObjectLinkTableSync>::TABLE(
			"t040_alarm_object_links"
		);

		template<> const Field DBTableSyncTemplate<AlarmObjectLinkTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(AlarmObjectLinkTableSync::COL_RECIPIENT_KEY, SQL_TEXT),
			Field(AlarmObjectLinkTableSync::COL_OBJECT_ID, SQL_INTEGER),
			Field(AlarmObjectLinkTableSync::COL_ALARM_ID, SQL_INTEGER),
			Field(AlarmObjectLinkTableSync::COL_PARAMETER, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<AlarmObjectLinkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(AlarmObjectLinkTableSync::COL_OBJECT_ID.c_str(),	AlarmObjectLinkTableSync::COL_ALARM_ID.c_str(), ""));
			r.push_back(DBTableSync::Index(AlarmObjectLinkTableSync::COL_ALARM_ID.c_str(), ""));
			return r;
		}

		template<>
		void OldLoadSavePolicy<AlarmObjectLinkTableSync,AlarmObjectLink>::Load(
			AlarmObjectLink* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// It makes no sense to load such an object without the up links
			assert(linkLevel > FIELDS_ONLY_LOAD_LEVEL);

			// Recipient type
			object->setRecipient(
				rows->get<string>( AlarmObjectLinkTableSync::COL_RECIPIENT_KEY)
			);

			// Parameter
			object->setParameter(
				rows->get<string>(AlarmObjectLinkTableSync::COL_PARAMETER)
			);

			// Object ID
			object->setObjectId(
				rows->getDefault<RegistryKeyType>(AlarmObjectLinkTableSync::COL_OBJECT_ID, 0)
			);

			// Message
			try
			{
				util::RegistryKeyType alarmId(rows->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID));
				boost::shared_ptr<Alarm> alarm;
				alarm = AlarmTableSync::GetEditable(
					alarmId,
					env,
					linkLevel
					);
				object->setAlarm(alarm.get());
				alarm->addLinkedObject(*object);
				alarm->clearBroadcastPointsCache();
			}
			catch(ObjectNotFoundException<Alarm> e)
			{
				throw LinkException<AlarmObjectLinkTableSync>(rows, AlarmObjectLinkTableSync::COL_ALARM_ID, e);
			}
		}



		template<>
		void OldLoadSavePolicy<AlarmObjectLinkTableSync,AlarmObjectLink>::Unlink(
			AlarmObjectLink* object
		){
			if(object->getAlarm())
			{
				object->getAlarm()->removeLinkedObject(*object);
				object->getAlarm()->clearBroadcastPointsCache();
			}
		}



		template<>
		void OldLoadSavePolicy<AlarmObjectLinkTableSync,AlarmObjectLink>::Save(
			AlarmObjectLink* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<AlarmObjectLinkTableSync> query(*object);
			query.addField(object->getRecipient()->getFactoryKey());
			query.addField(object->getObjectId());
			query.addField(object->getAlarm() ? object->getAlarm()->getKey() : RegistryKeyType(0));
			query.addField(object->getParameter());
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<AlarmObjectLinkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				boost::shared_ptr<const AlarmObjectLink> aol(AlarmObjectLinkTableSync::Get(object_id, env));
				if(aol->getAlarm()->belongsToTemplate())
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
				}
				else
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
				}
			}
			catch (ObjectNotFoundException<AlarmObjectLink>&)
			{
				return false;
			}
		}



		template<> void DBTableSyncTemplate<AlarmObjectLinkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlarmObjectLinkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlarmObjectLinkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const AlarmObjectLink> aol(AlarmObjectLinkTableSync::Get(id, env));
			if (aol->getAlarm()->belongsToTemplate())
			{
				MessagesLibraryLog::addUpdateEntry(
					aol->getAlarm(),
					"Suppression de la destination "+ lexical_cast<string>(aol->getObjectId()),
					session->getUser().get()
					);
			}
			else
			{
				MessagesLog::addUpdateEntry(
					aol->getAlarm(),
					"Suppression de la destination "+ lexical_cast<string>(aol->getObjectId()),
					session->getUser().get()
				);
			}
		}
	}

	namespace messages
	{
		void AlarmObjectLinkTableSync::RemoveByMessage(
			RegistryKeyType alarmId,
			optional<RegistryKeyType> objectId,
			optional<DBTransaction&> transaction
		){
			DeleteQuery<AlarmObjectLinkTableSync> query;
			query.addWhereField(COL_ALARM_ID, alarmId);
			if(objectId)
			{
				query.addWhereField(COL_OBJECT_ID, *objectId);
			}
			query.execute(transaction);
		}



		void AlarmObjectLinkTableSync::CopyRecipients(
			RegistryKeyType sourceId,
			Alarm& destAlarm,
			optional<DBTransaction&> transaction
		){
			Env lenv;
			SearchResult links(
				Search(lenv, sourceId)
			);
			BOOST_FOREACH(const boost::shared_ptr<AlarmObjectLink>& aol, links)
			{
				AlarmObjectLink naol;
				naol.setAlarm(&destAlarm);
				naol.setObjectId(aol->getObjectId());
				naol.setRecipient(aol->getRecipient()->getFactoryKey());
				naol.setParameter(aol->getParameter());
				Save(&naol, transaction);
			}
		}



		AlarmObjectLinkTableSync::SearchResult AlarmObjectLinkTableSync::Search(
			Env& env,
			util::RegistryKeyType alarmId,
			int first /*= 0*/,
			boost::optional<std::size_t> number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << alarmId;
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		void AlarmObjectLinkTableSync::RemoveByTarget(
			util::RegistryKeyType objectId,
			optional<DBTransaction&> transaction
		){
			DeleteQuery<AlarmObjectLinkTableSync> query;
			query.addWhereField(COL_OBJECT_ID, objectId);
			query.execute(transaction);
		}
}	}
