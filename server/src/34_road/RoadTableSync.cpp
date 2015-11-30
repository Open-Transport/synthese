/** RoadTableSync class implementation.
	@file RoadTableSync.cpp

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

#include "RoadTableSync.h"

#include "RoadPlace.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, RoadTableSync>::FACTORY_KEY(
			"34.10.01 Roads"
		);
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RoadTableSync>::TABLE(
			"t015_roads"
		);



		template<> const Field DBTableSyncTemplate<RoadTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<RoadTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<RoadTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<RoadTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace road
	{
		RoadTableSync::SearchResult RoadTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> roadPlaceId,
			boost::optional<util::RegistryKeyType> cityId,
//			boost::optional<util::RegistryKeyType> startingNodeId,
//			boost::optional<util::RegistryKeyType> endingNodeId,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if(roadPlaceId)
			{
				query << " AND " << RoadPlace::FIELD.name << "=" << *roadPlaceId;
			} /// @todo implementation
/*			if(startingNodeId)
			{
				query << " AND (SELECT " << RoadChunkTableSync::COL_CROSSING_ID << " FROM " << RoadChunkTableSync::TABLE.NAME << " WHERE " << RoadChunkTableSync::COL_ROADID << "=" << TABLE.NAME << "." << TABLE_COL_ID << " AND " << RoadChunkTableSync::COL_RANKINPATH << "=0)=" << *startingNodeId;
			}
			if(endingNodeId)
			{
				query << " AND (SELECT " << RoadChunkTableSync::COL_CROSSING_ID << " FROM " << RoadChunkTableSync::TABLE.NAME << " WHERE " << RoadChunkTableSync::COL_ROADID << "=" << TABLE.NAME << "." << TABLE_COL_ID << " ORDER BY " << RoadChunkTableSync::COL_RANKINPATH << " DESC LIMIT 1)=" << *startingNodeId;
			}
*/			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}

		bool RoadTableSync::allowList(const server::Session* session) const
		{
			return true;
		}		
	}
}
