
/** RoadChunkTableSync class implementation.
	@file RoadChunkTableSync.cpp

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

#include "RoadChunkTableSync.h"

#include "Address.h"
#include "CrossingTableSync.hpp"
#include "RoadModule.h"
#include "RoadTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "LinkException.h"
#include "CoordinatesSystem.hpp"
#include "RuleUser.h"
#include "GraphConstants.h"
#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"
#include "AccessParameters.h"
#include "RoadChunkEdge.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/geom/LineString.h>


using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace algorithm;
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, RoadChunkTableSync>::FACTORY_KEY("34.30.01 Road chunks");
	}

	namespace road
	{
		// TODO: rename column to crossing_id (see https://188.165.247.81/projects/synthese/wiki/Database_Schema_Updates).
		const string RoadChunkTableSync::COL_CROSSING_ID("address_id");
		const string RoadChunkTableSync::COL_RANKINPATH("rank_in_path");
		const string RoadChunkTableSync::COL_ROADID("road_id");  // NU
		const string RoadChunkTableSync::COL_METRICOFFSET("metric_offset");  // U ??
		const string RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER("left_start_house_number");
		const string RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER("left_end_house_number");
		const string RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER("right_start_house_number");
		const string RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER("right_end_house_number");
		const string RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY("left_house_numbering_policy");
		const string RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY("right_house_numbering_policy");
		const string RoadChunkTableSync::COL_ONE_WAY("one_way");
		const string RoadChunkTableSync::COL_CAR_SPEED("car_speed");
		const string RoadChunkTableSync::COL_NON_WALKABLE("non_walkable");
		const string RoadChunkTableSync::COL_NON_DRIVABLE("non_drivable");
		const string RoadChunkTableSync::COL_NON_BIKABLE("non_bikable");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RoadChunkTableSync>::TABLE(
			"t014_road_chunks"
		);

		template<> const Field DBTableSyncTemplate<RoadChunkTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_CROSSING_ID, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_RANKINPATH, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_ROADID, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_METRICOFFSET, SQL_DOUBLE),
			Field(RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_ONE_WAY, SQL_INTEGER),
			Field(RoadChunkTableSync::COL_CAR_SPEED, SQL_DOUBLE),
			Field(RoadChunkTableSync::COL_NON_WALKABLE, SQL_BOOLEAN),
			Field(RoadChunkTableSync::COL_NON_DRIVABLE, SQL_BOOLEAN),
			Field(RoadChunkTableSync::COL_NON_BIKABLE, SQL_BOOLEAN),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_LINESTRING),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<RoadChunkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					RoadChunkTableSync::COL_ROADID.c_str(),
					RoadChunkTableSync::COL_RANKINPATH.c_str(),
					""
			)	);
			return r;
		};

		template<> void OldLoadSavePolicy<RoadChunkTableSync,RoadChunk>::Load(
			RoadChunk* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(object->getLinkedObjectsIds(*rows), env, linkLevel);
			object->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				object->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void OldLoadSavePolicy<RoadChunkTableSync,RoadChunk>::Unlink(
			RoadChunk* obj
		){
			obj->unlink();
		}



		template<> void OldLoadSavePolicy<RoadChunkTableSync,RoadChunk>::Save(
			RoadChunk* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<RoadChunkTableSync> query(*object);
			query.addField(object->getFromCrossing() ? object->getFromCrossing()->getKey() : RegistryKeyType(0));
			query.addField(object->getRankInPath());
			query.addField(object->getRoad() ? object->getRoad()->getKey() : RegistryKeyType(0));
			query.addField(object->getMetricOffset());
			query.addField(object->getLeftHouseNumberBounds() ? lexical_cast<string>(object->getLeftHouseNumberBounds()->first) : string());
			query.addField(object->getLeftHouseNumberBounds() ? lexical_cast<string>(object->getLeftHouseNumberBounds()->second) : string());
			query.addField(object->getRightHouseNumberBounds() ? lexical_cast<string>(object->getRightHouseNumberBounds()->first) : string());
			query.addField(object->getRightHouseNumberBounds() ? lexical_cast<string>(object->getRightHouseNumberBounds()->second) : string());
			query.addField(static_cast<int>(object->getLeftHouseNumberBounds() ? object->getLeftHouseNumberingPolicy() : road::ALL_NUMBERS));
			query.addField(static_cast<int>(object->getRightHouseNumberBounds() ? object->getRightHouseNumberingPolicy() : road::ALL_NUMBERS));
			query.addField(object->getCarOneWay());
			query.addField(object->getCarSpeed(true));
			query.addField(object->getNonWalkable());
			query.addField(object->getNonDrivable());
			query.addField(object->getNonBikable());
			query.addField(dynamic_pointer_cast<Geometry,LineString>(object->getGeometry()));
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<RoadChunkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
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



		template<> void DBTableSyncTemplate<RoadChunkTableSync>::LogRemoval(
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
			optional<RegistryKeyType> roadId,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<RoadChunkTableSync> query;
			if(roadId)
			{
				query.addWhereField(COL_ROADID, *roadId);
			}
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
	    }



		void RoadChunkTableSync::ProjectAddress(
			const Point& point,
			double maxDistance,
			Address& address,
			EdgeProjector<boost::shared_ptr<RoadChunk> >::CompatibleUserClassesRequired requiredUserClasses
		){
			EdgeProjector<boost::shared_ptr<RoadChunk> >::From paths(
				SearchByMaxDistance(
					point,
					maxDistance,
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
			)	);

			if(!paths.empty())
			{
				EdgeProjector<boost::shared_ptr<RoadChunk> > projector(paths, maxDistance, requiredUserClasses);

				try
				{
					EdgeProjector<boost::shared_ptr<RoadChunk> >::PathNearby projection(
						projector.projectEdge(
							*point.getCoordinate()
					)	);

					address.setGeometry(
						boost::shared_ptr<Point>(
							CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createPoint(
								projection.get<0>()
					)	)	);
					address.setRoadChunk(projection.get<1>().get());
					address.setMetricOffset(projection.get<2>());
				}
				catch(EdgeProjector<boost::shared_ptr<RoadChunk> >::NotFoundException)
				{
				}
			}
		}
}	}
