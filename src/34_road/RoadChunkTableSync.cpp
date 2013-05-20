
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
#include "ReverseRoadChunk.hpp"

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

		template<> void OldLoadSavePolicy<RoadChunkTableSync,MainRoadChunk>::Load(
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
				object->setGeometry(boost::shared_ptr<LineString>());
			}
			else
			{
				object->setGeometry(
					dynamic_pointer_cast<LineString,Geometry>(rows->getGeometryFromWKT(TABLE_COL_GEOMETRY))
				);
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				bool noMotorVehicles(false);

				try
				{
					boost::shared_ptr<MainRoadPart> road(RoadTableSync::GetEditable(rows->getLongLong(RoadChunkTableSync::COL_ROADID), env, linkLevel));
					object->setRoad(road.get());
					object->setFromCrossing(
						CrossingTableSync::GetEditable(
							rows->getLongLong(RoadChunkTableSync::COL_CROSSING_ID),
							env,
							linkLevel
						).get()
					);
					road->addRoadChunk(*object);

					/* 
					TODO Backward compatibility.
					Useless for OpenStreetMap datas since non_drivable field in database (r8381)
					and after a fresh OSM import.
					Now the OSM importer check those fields (and some more) and save it into DB.
					Should be useless soon for OSM graph, might be still usefull for pure road
					journey planning on NAVTEQ graph (must be pretty rare).
					*/
					switch(road->getType())
					{
						case Road::ROAD_TYPE_PEDESTRIANSTREET:
						case Road::ROAD_TYPE_PEDESTRIANPATH:
						case Road::ROAD_TYPE_STEPS:
						case Road::ROAD_TYPE_PRIVATEWAY:
							noMotorVehicles = true;
						break;

						default:
							noMotorVehicles = false;
						break;
					}
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

				RuleUser::Rules rules(RuleUser::GetEmptyRules());
				rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();

				if(rows->getBool(RoadChunkTableSync::COL_NON_WALKABLE))
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				}				

				if(rows->getBool(RoadChunkTableSync::COL_NON_BIKABLE))
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				}				

				int oneWay = rows->getInt(RoadChunkTableSync::COL_ONE_WAY);
				if(rows->getBool(RoadChunkTableSync::COL_NON_DRIVABLE) || noMotorVehicles)
				{
					rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
					object->getReverseRoadChunk()->setRules(rules);
					object->setRules(rules);
				}
				else if(oneWay == 1)
				{
					object->setRules(rules);
					rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
					object->getReverseRoadChunk()->setRules(rules);
				}
				else if(oneWay == -1)
				{
					object->getReverseRoadChunk()->setRules(rules);
					rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
					object->setRules(rules);
				}
				else
				{
					object->getReverseRoadChunk()->setRules(rules);
					object->setRules(rules);
				}

				double maxSpeed = rows->getDouble(RoadChunkTableSync::COL_CAR_SPEED);
				object->setCarSpeed(maxSpeed);
				object->getReverseRoadChunk()->setCarSpeed(maxSpeed);

				if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
				{
					// Useful transfer calculation
					object->getHub()->clearAndPropagateUsefulTransfer(RoadModule::GRAPH_ID);
				}
			}
		}



		template<> void OldLoadSavePolicy<RoadChunkTableSync,MainRoadChunk>::Unlink(
			MainRoadChunk* obj
		){
			// Useful transfer calculation
			if(obj->getHub())
			{
				obj->getHub()->clearAndPropagateUsefulTransfer(RoadModule::GRAPH_ID);
			}
		}



		template<> void OldLoadSavePolicy<RoadChunkTableSync,MainRoadChunk>::Save(
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

			AccessParameters ac(USER_CAR);
			int oneWay = 0;
			bool mainCarAllowance = object->getUseRule(USER_CAR - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac);
			bool reverseCarAllowance = object->getReverseRoadChunk()->getUseRule(USER_CAR - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac);
			if(!mainCarAllowance && reverseCarAllowance)
				oneWay = -1;
			else if(mainCarAllowance && !reverseCarAllowance)
				oneWay = 1;
			query.addField(oneWay);
			query.addField(object->getCarSpeed(true));
			ac = AccessParameters(USER_PEDESTRIAN);
			query.addField(!object->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac));
			query.addField(!mainCarAllowance && !reverseCarAllowance);
			ac = AccessParameters(USER_BIKE);
			query.addField(!object->getUseRule(USER_BIKE - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac));
			query.addField(static_pointer_cast<Geometry,LineString>(object->getGeometry()));
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
			EdgeProjector<boost::shared_ptr<MainRoadChunk> >::CompatibleUserClassesRequired requiredUserClasses
		){
			EdgeProjector<boost::shared_ptr<MainRoadChunk> >::From paths(
				SearchByMaxDistance(
					point,
					maxDistance,
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
			)	);

			if(!paths.empty())
			{
				EdgeProjector<boost::shared_ptr<MainRoadChunk> > projector(paths, maxDistance, requiredUserClasses);

				try
				{
					EdgeProjector<boost::shared_ptr<MainRoadChunk> >::PathNearby projection(
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
				catch(EdgeProjector<boost::shared_ptr<MainRoadChunk> >::NotFoundException)
				{
				}
			}
		}
}	}
