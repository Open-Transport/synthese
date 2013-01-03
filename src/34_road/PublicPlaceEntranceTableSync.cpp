
//////////////////////////////////////////////////////////////////////////
///	PublicPlaceEntranceTableSync class implementation.
///	@file PublicPlaceEntranceTableSync.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PublicPlaceEntranceTableSync.hpp"

#include "Crossing.h"
#include "ImportableTableSync.hpp"
#include "PublicPlaceTableSync.h"
#include "RoadChunkTableSync.h"
#include "SelectQuery.hpp"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,PublicPlaceEntranceTableSync>::FACTORY_KEY("34.50 Public place entrances");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PublicPlaceEntranceTableSync>::TABLE(
			"t084_public_place_entrances"
		);



		template<> const Field DBTableSyncTemplate<PublicPlaceEntranceTableSync>::_FIELDS[]={ Field() };



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<PublicPlaceEntranceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					SimpleObjectFieldDefinition<PublicPlace>::FIELD.name.c_str(),
			"")	);
			return r;
		};



		template<> bool DBTableSyncTemplate<PublicPlaceEntranceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<PublicPlaceEntranceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PublicPlaceEntranceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PublicPlaceEntranceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace road
	{
		PublicPlaceEntranceTableSync::SearchResult PublicPlaceEntranceTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<PublicPlaceEntranceTableSync> query;
			// if(parameterId)
			// {
			// 	query.addWhereField(COL_PARENT_ID, *parentFolderId);
			// }
			// if(orderByName)
			// {
			// 	query.addOrderField(COL_NAME, raisingOrder);
			// }
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
}	}
