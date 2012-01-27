
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
#include "ReplaceQuery.h"
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
	using namespace security;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,PublicPlaceEntranceTableSync>::FACTORY_KEY("34.50 Public place entrances");
	}

	namespace road
	{
		const string PublicPlaceEntranceTableSync::COL_PUBLIC_PLACE_ID = "public_place_id";
		const string PublicPlaceEntranceTableSync::COL_NAME = "name";
		const string PublicPlaceEntranceTableSync::COL_ROAD_CHUNK_ID = "road_chunk_id";
		const string PublicPlaceEntranceTableSync::COL_METRIC_OFFSET = "metric_offset";
		const string PublicPlaceEntranceTableSync::COL_NUMBER = "number";
		const string PublicPlaceEntranceTableSync::COL_DATASOURCE_LINKS = "datasource_links";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PublicPlaceEntranceTableSync>::TABLE(
			"t084_public_place_entrances"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<PublicPlaceEntranceTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(PublicPlaceEntranceTableSync::COL_PUBLIC_PLACE_ID, SQL_INTEGER),
			DBTableSync::Field(PublicPlaceEntranceTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(PublicPlaceEntranceTableSync::COL_ROAD_CHUNK_ID, SQL_INTEGER),
			DBTableSync::Field(PublicPlaceEntranceTableSync::COL_METRIC_OFFSET, SQL_DOUBLE),
			DBTableSync::Field(PublicPlaceEntranceTableSync::COL_NUMBER, SQL_INTEGER),
			DBTableSync::Field(PublicPlaceEntranceTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<PublicPlaceEntranceTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				PublicPlaceEntranceTableSync::COL_PUBLIC_PLACE_ID.c_str(),
			""),
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<PublicPlaceEntranceTableSync,PublicPlaceEntrance>::Load(
			PublicPlaceEntrance* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Public place
			object->setPublicPlace(NULL);
			if (linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				shared_ptr<PublicPlace> publicPlace(
					PublicPlaceTableSync::GetEditable(
						rows->getLongLong(PublicPlaceEntranceTableSync::COL_PUBLIC_PLACE_ID),
						env,
						linkLevel
				)	);
				object->setPublicPlace(publicPlace.get());

				publicPlace->addEntrance(*object);
			}

			// Name
			object->setName(rows->getText(PublicPlaceEntranceTableSync::COL_NAME));

			// Address
			RegistryKeyType chunkId(rows->getLongLong(PublicPlaceEntranceTableSync::COL_ROAD_CHUNK_ID));
			object->setRoadChunk(NULL);
			object->setMetricOffset(0);
			if(chunkId > 0)
			{
				// Road chunk
				try
				{
					object->setRoadChunk(
						RoadChunkTableSync::GetEditable(chunkId, env, linkLevel).get()
					);
				}
				catch (ObjectNotFoundException<MainRoadChunk>&)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(chunkId) + " for projected chunk in stop " + lexical_cast<string>(object->getKey()));
				}

				// Metric offset
				object->setMetricOffset(rows->getDouble(PublicPlaceEntranceTableSync::COL_METRIC_OFFSET));
			}

			// House number
			MainRoadChunk::HouseNumber houseNumber = rows->getInt(PublicPlaceEntranceTableSync::COL_NUMBER);
			object->setHouseNumber(
				houseNumber ?
				optional<MainRoadChunk::HouseNumber>(houseNumber) :
				optional<MainRoadChunk::HouseNumber>()
			);

			// Datasource links
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				object->setDataSourceLinksWithoutRegistration(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(PublicPlaceEntranceTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
			}
		}



		template<> void DBDirectTableSyncTemplate<PublicPlaceEntranceTableSync,PublicPlaceEntrance>::Save(
			PublicPlaceEntrance* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<PublicPlaceEntranceTableSync> query(*object);
			query.addField(object->getPublicPlace() ? object->getPublicPlace()->getKey() : RegistryKeyType(0));
			query.addField(object->getName());
			query.addField(object->getRoadChunk() ? object->getRoadChunk()->getKey() : RegistryKeyType(0));
			query.addField(object->getMetricOffset());
			query.addField(object->getHouseNumber() ? lexical_cast<string>(*object->getHouseNumber()) : string());
			query.addField(ImportableTableSync::SerializeDataSourceLinks(object->getDataSourceLinks()));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<PublicPlaceEntranceTableSync,PublicPlaceEntrance>::Unlink(
			PublicPlaceEntrance* obj
		){
			// Public place link
			if(obj->getPublicPlace())
			{
				obj->getPublicPlace()->removeEntrance(*obj);
			}
		}



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
