/** NotificationProviderTableSync implementation.
	@file NotificationProviderTableSync.cpp
	@author Yves Martin
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

#include <NotificationProviderTableSync.hpp>

#include <DBTableSync.hpp>
#include <DBTableSyncTemplate.hpp>
#include <FactorableTemplate.h>
#include <Field.hpp>

#include <Session.h>
#include <Registry.h>

#include <string>

namespace synthese
{
	using namespace messages;

	namespace util
	{
		template<> const std::string FactorableTemplate<db::DBTableSync,NotificationProviderTableSync>::FACTORY_KEY(
			"17.10 Notification providers"
		);
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<NotificationProviderTableSync>::TABLE(
			"t199_notification_providers"
		);

		template<> const Field DBTableSyncTemplate<NotificationProviderTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<NotificationProviderTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<NotificationProviderTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			//TODO Prevent removal if NotificationEvent instances still exist
			//with reference to it
			return true;
		}



		template<> void DBTableSyncTemplate<NotificationProviderTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<NotificationProviderTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<NotificationProviderTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace messages
	{
		NotificationProviderTableSync::NotificationProviderTableSync() {
			// TODO Auto-generated constructor stub
		}

		NotificationProviderTableSync::~NotificationProviderTableSync() {
			// TODO Auto-generated destructor stub
		}

	} /* namespace messages */
} /* namespace synthese */
