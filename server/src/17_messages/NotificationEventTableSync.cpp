/** NotificationEventTableSync class implementation.
	@file NotificationEventTableSync.cpp
	@author Ivan Ivanov
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "NotificationEventTableSync.hpp"

#include "DBResult.hpp"
#include "SelectQuery.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, NotificationEventTableSync>::FACTORY_KEY("17.10 Notification Events");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<NotificationEventTableSync>::TABLE(
			"t121_notification_events"
			);

		template<> const Field DBTableSyncTemplate<NotificationEventTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<NotificationEventTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<NotificationEventTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
			){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<NotificationEventTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
			){
		}



		template<> void DBTableSyncTemplate<NotificationEventTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
			){
		}



		template<> void DBTableSyncTemplate<NotificationEventTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
			){
			//TODO Log the removal
		}
	}

	namespace messages
	{
		NotificationEventTableSync::SearchResult NotificationEventTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> alarmId,
			boost::optional<util::RegistryKeyType> notificationProviderId,
			int first,
			boost::optional<std::size_t> number,
			bool orderByLastAttempt,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<NotificationEventTableSync> query;

			if (alarmId)
			{
				query.addWhereField(Alarm::FIELD.name, *alarmId);
			}
			if (notificationProviderId)
			{
				query.addWhereField(NotificationProvider::FIELD.name, *notificationProviderId);
			}
			if (orderByLastAttempt)
			{
				query.addOrderField(SimpleObjectFieldDefinition<LastAttempt>::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
