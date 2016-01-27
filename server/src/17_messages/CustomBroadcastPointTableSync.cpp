
/** CustomBroadcastPointTableSync class implementation.
	@file CustomBroadcastPointTableSync.cpp

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

#include "CustomBroadcastPointTableSync.hpp"

#include "DBResult.hpp"

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
		template<> const string FactorableTemplate<DBTableSync,CustomBroadcastPointTableSync>::FACTORY_KEY("17.50 Custom broadcast points");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CustomBroadcastPointTableSync>::TABLE(
			"t106_custom_broadcast_points"
		);

		template<> const Field DBTableSyncTemplate<CustomBroadcastPointTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CustomBroadcastPointTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<CustomBroadcastPointTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<CustomBroadcastPointTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CustomBroadcastPointTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CustomBroadcastPointTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace messages
	{
		CustomBroadcastPointTableSync::SearchResult CustomBroadcastPointTableSync::Search(
			Env& env,
			std::string name,
			boost::optional<util::RegistryKeyType> parentId,
			int first /*= 0*/,
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		)
		{
			CustomBroadcastPointTableSync::SearchResult result;
			CustomBroadcastPointTableSync::Search(env, std::back_inserter(result),
										  name, parentId, first, number, orderByName, raisingOrder, linkLevel);
			return result;
		}

}	}
