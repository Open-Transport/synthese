
/** HouseTableSync class implementation.
	@file HouseTableSync.cpp

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

#include "HouseTableSync.hpp"

#include "EdgeProjector.hpp"
#include "House.hpp"
#include "RoadChunk.h"
#include "DBModule.h"
#include "LinkException.h"
#include "DataSource.h"
#include "DataSourceLinksField.hpp"
#include "DataSourceTableSync.h"
#include "SelectQuery.hpp"
#include "ReplaceQuery.h"
#include "ImportableTableSync.hpp"
#include "Road.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"

#include <sstream>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace algorithm;
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace impex;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, HouseTableSync>::FACTORY_KEY("34.40.01 House");
	}

	namespace road
	{
		const std::string HouseTableSync::COL_ROAD_PLACE_ID ("road_place_id");
		const std::string HouseTableSync::COL_DATA_SOURCE_LINKS ("data_source_links");
		const std::string HouseTableSync::COL_NUMBER ("number");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<HouseTableSync>::TABLE(
			"t078_houses"
		);

		template<> const Field DBTableSyncTemplate<HouseTableSync>::_FIELDS[] =
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(HouseTableSync::COL_DATA_SOURCE_LINKS, SQL_TEXT),
			Field(HouseTableSync::COL_ROAD_PLACE_ID, SQL_INTEGER),
			Field(HouseTableSync::COL_NUMBER, SQL_INTEGER),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<HouseTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}

		template<> void OldLoadSavePolicy<HouseTableSync, House>::Load(
			House* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Geometry
			boost::shared_ptr<Point> point(
				dynamic_pointer_cast<Point, Geometry>(
					rows->getGeometryFromWKT(TABLE_COL_GEOMETRY)
			)	);
			if(point.get())
			{
				object->setGeometry(point);
			}

			// Code by source
			object->setDataSourceLinksWithoutRegistration(
				ImportableTableSync::GetDataSourceLinksFromSerializedString(
					rows->getText(HouseTableSync::COL_DATA_SOURCE_LINKS),
					env
			)	);

			// House number
			HouseNumber houseNumber = rows->getInt(HouseTableSync::COL_NUMBER);
			object->setHouseNumber(houseNumber);

			// Road Place
			RegistryKeyType roadId(
				rows->getLongLong(HouseTableSync::COL_ROAD_PLACE_ID)
			);
			try
			{
				boost::shared_ptr<RoadPlace> roadPlace(RoadPlaceTableSync::GetEditable(roadId, env));
				object->setRoadChunkFromRoadPlace(roadPlace);
			}
			catch(ObjectNotFoundException<RoadPlace>&)
			{
				Log::GetInstance().warn("No such road place "+ lexical_cast<string>(roadId) +" in house "+ lexical_cast<string>(object->getKey()));
				throw;
			}
			catch(EdgeProjector<RoadChunk*>::NotFoundException&)
			{
				Log::GetInstance().warn("No chunk was found near the house "+ lexical_cast<string>(object->getKey()) +" in the road place "+ lexical_cast<string>(roadId));
				throw;
			}
		}



		template<> void OldLoadSavePolicy<HouseTableSync, House>::Unlink(
			House* obj
		){
		}



		template<> void OldLoadSavePolicy<HouseTableSync, House>::Save(
			House* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<HouseTableSync> query(*object);
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.addField(object->getRoadChunk() ? object->getRoadChunk()->getRoad()->get<RoadPlace>()->getKey() : RegistryKeyType(0));
			query.addField(object->getHouseNumber() ? static_cast<int>(*(object->getHouseNumber())) : 0);
			query.addField(static_pointer_cast<Geometry,Point>(object->getGeometry()));
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<HouseTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<HouseTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<HouseTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<HouseTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}
}
