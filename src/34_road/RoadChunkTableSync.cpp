
/** RoadChunkTableSync class implementation.
	@file RoadChunkTableSync.cpp

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

#include "RoadChunkTableSync.h"
#include "CrossingTableSync.hpp"
#include "RoadTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "LinkException.h"
#include "CoordinatesSystem.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/geom/LineString.h>


using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, RoadChunkTableSync>::FACTORY_KEY("34.30.01 Road chunks");
	}

	namespace road
	{
		const string RoadChunkTableSync::COL_CROSSING_ID("address_id");
		const string RoadChunkTableSync::COL_RANKINPATH ("rank_in_path");
		const string RoadChunkTableSync::COL_ROADID ("road_id");  // NU
		const string RoadChunkTableSync::COL_METRICOFFSET ("metric_offset");  // U ??
		const string RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER("left_start_house_number");
		const string RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER("left_end_house_number");
		const string RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER("right_start_house_number");
		const string RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER("right_end_house_number");
		const string RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY("left_house_numbering_policy");
		const string RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY("right_house_numbering_policy");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RoadChunkTableSync>::TABLE(
			"t014_road_chunks"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<RoadChunkTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_CROSSING_ID, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_RANKINPATH, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_ROADID, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_METRICOFFSET, SQL_DOUBLE),
			DBTableSync::Field(RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY, SQL_INTEGER),
			DBTableSync::Field(RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY, SQL_INTEGER),
			DBTableSync::Field(TABLE_COL_GEOMETRY, SQL_GEOM_LINESTRING),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<RoadChunkTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				RoadChunkTableSync::COL_ROADID.c_str(),
				RoadChunkTableSync::COL_RANKINPATH.c_str(),
				""
			),
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<RoadChunkTableSync,MainRoadChunk>::Load(
			MainRoadChunk* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    // Rank in road
		    int rankInRoad (rows->getInt (RoadChunkTableSync::COL_RANKINPATH));
		    object->setRankInPath(rankInRoad);
			
			// Metric offset
			object->setMetricOffset(rows->getDouble (RoadChunkTableSync::COL_METRICOFFSET));

		    // Geometry
			string viaPointsStr(rows->getText (TABLE_COL_GEOMETRY));
			if(viaPointsStr.empty())
			{
				object->setGeometry(shared_ptr<LineString>());
			}
			else
			{
				object->setGeometry(
					dynamic_pointer_cast<LineString,Geometry>(rows->getGeometryFromWKT(TABLE_COL_GEOMETRY))
				);
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					shared_ptr<MainRoadPart> road(RoadTableSync::GetEditable(rows->getLongLong(RoadChunkTableSync::COL_ROADID), env, linkLevel));
					object->setRoad(road.get());
					object->setFromCrossing(
						CrossingTableSync::GetEditable(
							rows->getLongLong(RoadChunkTableSync::COL_CROSSING_ID),
							env,
							linkLevel
						).get()
					);
					road->addRoadChunk(*object);
				}
				catch (ObjectNotFoundException<Road>& e)
				{
					throw LinkException<RoadChunkTableSync>(rows, RoadChunkTableSync::COL_ROADID, e);
				}
				catch (ObjectNotFoundException<Crossing>& e)
				{
					throw LinkException<RoadChunkTableSync>(rows, RoadChunkTableSync::COL_CROSSING_ID, e);
				}

				// Left house number bounds
				if(	!rows->getText(RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER).empty() &&
					!rows->getText(RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER).empty()
				){
					object->setLeftHouseNumberBounds(
						MainRoadChunk::HouseNumberBounds(MainRoadChunk::HouseNumberBounds::value_type(
							rows->getInt(RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER),
							rows->getInt(RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER)
					)	));

					// Left House numbering policy
					object->setLeftHouseNumberingPolicy(
						static_cast<MainRoadChunk::HouseNumberingPolicy>(rows->getInt(RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY))
					);
				}


				// Right house number bounds
				if(	!rows->getText(RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER).empty() &&
					!rows->getText(RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER).empty()
				){
					object->setRightHouseNumberBounds(
						MainRoadChunk::HouseNumberBounds(MainRoadChunk::HouseNumberBounds::value_type(
							rows->getInt(RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER),
							rows->getInt(RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER)
					)	));

					// Right House numbering policy
					object->setRightHouseNumberingPolicy(
						static_cast<MainRoadChunk::HouseNumberingPolicy>(rows->getInt(RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY))
					);
				}
			}
		}



		template<> void DBDirectTableSyncTemplate<RoadChunkTableSync,MainRoadChunk>::Unlink(
			MainRoadChunk* obj
		){
		}



		template<> void DBDirectTableSyncTemplate<RoadChunkTableSync,MainRoadChunk>::Save(
			MainRoadChunk* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<RoadChunkTableSync> query(*object);
			query.addField(object->getFromCrossing() ? object->getFromCrossing()->getKey() : RegistryKeyType(0));
			query.addField(object->getRankInPath());
			query.addField(object->getMainRoadPart() ? object->getMainRoadPart()->getKey() : RegistryKeyType(0));
			query.addField(object->getMetricOffset());
			query.addField(object->getLeftHouseNumberBounds() ? lexical_cast<string>(object->getLeftHouseNumberBounds()->first) : string());
			query.addField(object->getLeftHouseNumberBounds() ? lexical_cast<string>(object->getLeftHouseNumberBounds()->second) : string());
			query.addField(object->getRightHouseNumberBounds() ? lexical_cast<string>(object->getRightHouseNumberBounds()->first) : string());
			query.addField(object->getRightHouseNumberBounds() ? lexical_cast<string>(object->getRightHouseNumberBounds()->second) : string());
			query.addField(static_cast<int>(object->getLeftHouseNumberBounds() ? object->getLeftHouseNumberingPolicy() : MainRoadChunk::ALL));
			query.addField(static_cast<int>(object->getRightHouseNumberBounds() ? object->getRightHouseNumberingPolicy() : MainRoadChunk::ALL));
			query.addField(static_pointer_cast<Geometry,LineString>(object->getGeometry()));
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<RoadChunkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Control user rights
			return true;
		}



		template<> void DBTableSyncTemplate<RoadChunkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadChunkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		void DBTableSyncTemplate<RoadChunkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace road
	{
	    RoadChunkTableSync::SearchResult RoadChunkTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<RoadChunkTableSync> query;
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
	    }
}	}
