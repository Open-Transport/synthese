
/** MainRoadChunk class implementation.
	@file MainRoadChunk.cpp

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

#include "MainRoadChunk.hpp"

#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"
#include "RoadChunkTableSync.h"
#include "RoadModule.h"
#include "RoadTableSync.h"
#include "CrossingTableSync.hpp"
#include "Registry.h"
#include "ReverseRoadChunk.hpp"
#include "MainRoadPart.hpp"
#include "House.hpp"

using namespace boost;
using namespace std;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace graph;
	using namespace util;

	namespace util
	{
		template<> const string Registry<road::MainRoadChunk>::KEY("MainRoadChunk");
	}

	namespace road
	{
		const MainRoadChunk::HouseNumber MainRoadChunk::HouseNumber_MAX(UINT_MAX);

		MainRoadChunk::MainRoadChunk(
			util::RegistryKeyType id /*= 0*/,
			Crossing* fromCrossing /*= NULL*/,
			int rankInRoad /*= UNKNOWN_VALUE*/,
			MainRoadPart* road /*= NULL*/,
			double metricOffset /*= UNKNOWN_VALUE */
		):	Registrable(id),
			RoadChunk(
				id,
				fromCrossing,
				rankInRoad,
				road,
				metricOffset
			),
			_carOneWay(false),
			_reverseRoadChunk(NULL)
		{}



		MainRoadChunk::~MainRoadChunk()
		{
			unlink();
		}



		bool MainRoadChunk::_testIfHouseNumberBelongsToBounds(
			const HouseNumberBounds& bounds,
			const HouseNumberingPolicy& policy,
			HouseNumber houseNumber
		){
			if(!bounds)
			{
				return false;
			}

			if( (	bounds->first <= bounds->second &&
					(houseNumber < bounds->first || houseNumber > bounds->second)
				) || (
					bounds->first >= bounds->second &&
					(houseNumber > bounds->first || houseNumber < bounds->second)
			)	)
			{
				return false;
			}

			switch(policy)
			{
			case ALL: return true;
			case ODD: return houseNumber % 2;
			case EVEN: return !(houseNumber % 2);
			}

			// Never execute
			return false;
		}


		bool MainRoadChunk::testIfHouseNumberBelongsToChunk(
			HouseNumber houseNumber
		) const	{
			return
				_testIfHouseNumberBelongsToBounds(_leftHouseNumberBounds, _leftHouseNumberingPolicy, houseNumber) ||
				_testIfHouseNumberBelongsToBounds(_rightHouseNumberBounds, _rightHouseNumberingPolicy, houseNumber) ||
				_houses.count(houseNumber);
			;
		}


		MetricOffset MainRoadChunk::_computeMetricOffset(
			const HouseNumberBounds& bounds,
			HouseNumber houseNumber,
			MetricOffset startOffset,
			MetricOffset endOffset
		){
			assert(bounds);
			assert(startOffset <= endOffset);

			if(startOffset == endOffset || bounds->first == bounds->second)
			{
				return startOffset;
			}

			return
				startOffset + (endOffset - startOffset) * (
					(bounds->first < bounds->second) ?
					((houseNumber - bounds->first) / (bounds->second - bounds->first)) :
					((bounds->first - houseNumber) / (bounds->first - bounds->second))
				)
			;
		}



		MetricOffset MainRoadChunk::getHouseNumberMetricOffset(
			HouseNumber houseNumber
		) const	{

			// Left side
			if(	_testIfHouseNumberBelongsToBounds(_leftHouseNumberBounds, _leftHouseNumberingPolicy, houseNumber))
			{
				return _computeMetricOffset(_leftHouseNumberBounds, houseNumber, getMetricOffset(), getEndMetricOffset());
			}

			// Right side
			if(	_testIfHouseNumberBelongsToBounds(_rightHouseNumberBounds, _rightHouseNumberingPolicy, houseNumber))
			{
				return _computeMetricOffset(_rightHouseNumberBounds, houseNumber, getMetricOffset(), getEndMetricOffset());
			}

			return getMetricOffset();
		}



		MainRoadPart* MainRoadChunk::getMainRoadPart() const
		{
			return static_cast<MainRoadPart*>(getRoad());
		}



		void MainRoadChunk::addHouse(House& house)
		{
			if(house.getHouseNumber())
			{
				_houses[*house.getHouseNumber()] = &house;
			}
		}



		void MainRoadChunk::removeHouse(House& house)
		{
			if(house.getHouseNumber())
			{
				_houses.erase(*house.getHouseNumber());
			}
		}



		bool MainRoadChunk::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Rank in road
			size_t rankInRoad(record.getDefault<size_t>(RoadChunkTableSync::COL_RANKINPATH, 0));
			if(rankInRoad != getRankInPath())
			{
				setRankInPath(rankInRoad);
				result = true;
			}

			// Metric offset
			{
				MetricOffset value(
					record.getDefault<MetricOffset>(RoadChunkTableSync::COL_METRICOFFSET, 0)
				);
				if(value != getMetricOffset())
				{
					setMetricOffset(value);
					result = true;
				}
			}

			// Geometry
			{
				string viaPointsStr(record.getDefault<string>(TABLE_COL_GEOMETRY));
				boost::shared_ptr<LineString> value(
					viaPointsStr.empty() ?
					boost::shared_ptr<LineString>() :
					dynamic_pointer_cast<LineString, geos::geom::Geometry>(record.getGeometryFromWKT(TABLE_COL_GEOMETRY))
				);
				if( (!value && getGeometry()) ||
					(value && !getGeometry()) ||
					(value && getGeometry() && !value->equalsExact(getGeometry().get(), 0.01))
				){
					setGeometry(value);
					result = true;
				}
			}

			// Road
			{
				boost::shared_ptr<MainRoadPart> value;
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(RoadChunkTableSync::COL_ROADID, 0)
				);
				if(id) try
				{
					value = RoadTableSync::GetEditable(id, env);
				}
				catch(ObjectNotFoundException<Road>& e)
				{
					Log::GetInstance().warn(
						"There was a link error in "+ RoadChunkTableSync::TABLE.NAME +
						" table at row "+ record.get<string>(TABLE_COL_ID) +
						" when loading road "+ lexical_cast<string>(id)	+" : "+ e.getMessage()
					);
				}
				if(value.get() != getRoad())
				{
					setRoad(value.get());
					result = true;
				}
			}

			// Crossing
			{
				boost::shared_ptr<Crossing> value;
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(RoadChunkTableSync::COL_CROSSING_ID, 0)
				);
				if(id) try
				{
					value = CrossingTableSync::GetEditable(id, env);
				}
				catch(ObjectNotFoundException<Crossing>& e)
				{
					Log::GetInstance().warn(
						"There was a link error in "+ RoadChunkTableSync::TABLE.NAME +
						" table at row "+ record.get<string>(TABLE_COL_ID) +
						" when loading crossing "+ lexical_cast<string>(id)	+" : "+ e.getMessage()
					);
				}
				if(value.get() != getFromCrossing())
				{
					setFromCrossing(value.get());
					result = true;
				}
			}
			
			// Left house number bounds
			{
				HouseNumberBounds::value_type value;
				HouseNumberingPolicy policyValue(ALL);

				if(	!record.getDefault<string>(RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER).empty() &&
					!record.getDefault<string>(RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER).empty()
				){
					value = make_pair(
						record.getDefault<HouseNumber>(RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER, 0),
						record.getDefault<HouseNumber>(RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER, 0)
					);
					policyValue = static_cast<HouseNumberingPolicy>(record.getDefault<int>(RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY));
				}
				if(value != getLeftHouseNumberBounds())
				{
					setLeftHouseNumberBounds(value);
					result = true;
				}
				if(policyValue != getLeftHouseNumberingPolicy())
				{
					setLeftHouseNumberingPolicy(policyValue);
					result = true;
				}
			}

			// Right house number bounds
			{
				HouseNumberBounds::value_type value;
				HouseNumberingPolicy policyValue(ALL);

				if(	!record.getDefault<string>(RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER).empty() &&
					!record.getDefault<string>(RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER).empty()
				){
					value = make_pair(
						record.getDefault<HouseNumber>(RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER, 0),
						record.getDefault<HouseNumber>(RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER, 0)
					);
					policyValue = static_cast<HouseNumberingPolicy>(record.getDefault<int>(RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY));
				}
				if(value != getRightHouseNumberBounds())
				{
					setRightHouseNumberBounds(value);
					result = true;
				}
				if(policyValue != getRightHouseNumberingPolicy())
				{
					setRightHouseNumberingPolicy(policyValue);
					result = true;
				}
			}

			// One way for cars
			{
				int value(record.getDefault<int>(RoadChunkTableSync::COL_ONE_WAY, 0));
				if(value != getCarOneWay())
				{
					setCarOneWay(value);
					result = true;
				}
			}

			// Use rules
			{
				bool noMotorVehicles(false);
				if(getRoad())
				{
					/* 
					TODO Backward compatibility.
					Useless for OpenStreetMap data since non_drivable field in database (r8381)
					and after a fresh OSM import.
					Now the OSM importer check those fields (and some more) and save it into DB.
					Should be useless soon for OSM graph, might be still useful for pure road
					journey planning on NAVTEQ graph (must be pretty rare).
					*/
					switch(getRoad()->getType())
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

				RuleUser::Rules rules(RuleUser::GetEmptyRules());
				rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				if(record.getDefault<bool>(RoadChunkTableSync::COL_NON_WALKABLE, false))
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				}				

				if(record.getDefault<bool>(RoadChunkTableSync::COL_NON_BIKABLE, false))
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				}				

				if(record.getDefault<bool>(RoadChunkTableSync::COL_NON_DRIVABLE, false) || noMotorVehicles || getCarOneWay() == -1)
				{
					rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				}
				if(	rules != getRules()
				){
					setRules(rules);
					result = true;
				}
			}

			// Max speed
			{
				double maxSpeed = record.getDefault<double>(RoadChunkTableSync::COL_CAR_SPEED, 0);
				if(maxSpeed != getCarSpeed())
				{
					setCarSpeed(maxSpeed);
					result = true;
				}
			}

			return result;
		}



		void MainRoadChunk::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(getMainRoadPart())
			{
				getMainRoadPart()->addRoadChunk(*this);

				// Update of the reverse road chunk
				getReverseRoadChunk()->setCarSpeed(getCarSpeed());
				RuleUser::Rules rules(getRules());
				if(getCarOneWay() == 1)
				{
					rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				}
				else if(getCarOneWay() == -1)
				{
					rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
				}
				getReverseRoadChunk()->setRules(rules);
			}

			// Useful transfer calculation
			if(getHub())
			{
				getHub()->clearAndPropagateUsefulTransfer(RoadModule::GRAPH_ID);
			}
		}



		void MainRoadChunk::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(prefix + RoadChunkTableSync::COL_CROSSING_ID, getFromCrossing() ? getFromCrossing()->getKey() : RegistryKeyType(0));
			pm.insert(prefix + RoadChunkTableSync::COL_RANKINPATH, getRankInPath());
			pm.insert(prefix + RoadChunkTableSync::COL_ROADID, getMainRoadPart() ? getMainRoadPart()->getKey() : RegistryKeyType(0));
			pm.insert(prefix + RoadChunkTableSync::COL_METRICOFFSET, getMetricOffset());
			pm.insert(prefix + RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER, getLeftHouseNumberBounds() ? lexical_cast<string>(getLeftHouseNumberBounds()->first) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER, getLeftHouseNumberBounds() ? lexical_cast<string>(getLeftHouseNumberBounds()->second) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER, getRightHouseNumberBounds() ? lexical_cast<string>(getRightHouseNumberBounds()->first) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER, getRightHouseNumberBounds() ? lexical_cast<string>(getRightHouseNumberBounds()->second) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY, static_cast<int>(getLeftHouseNumberBounds() ? getLeftHouseNumberingPolicy() : ALL));
			pm.insert(prefix + RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY, static_cast<int>(getRightHouseNumberBounds() ? getRightHouseNumberingPolicy() : ALL));

			AccessParameters ac(USER_CAR);
			bool mainCarAllowance = getUseRule(USER_CAR - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac);
			bool reverseCarAllowance = getReverseRoadChunk()->getUseRule(USER_CAR - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac);
			pm.insert(prefix + RoadChunkTableSync::COL_ONE_WAY, getCarOneWay());
			pm.insert(prefix + RoadChunkTableSync::COL_CAR_SPEED, getCarSpeed(true));
			ac = AccessParameters(USER_PEDESTRIAN);
			pm.insert(prefix + RoadChunkTableSync::COL_NON_WALKABLE, !getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac));
			pm.insert(prefix + RoadChunkTableSync::COL_NON_DRIVABLE, !mainCarAllowance && !reverseCarAllowance);
			ac = AccessParameters(USER_BIKE);
			pm.insert(prefix + RoadChunkTableSync::COL_NON_BIKABLE, !getUseRule(USER_BIKE - USER_CLASS_CODE_OFFSET).isCompatibleWith(ac));

			if(hasGeometry())
			{
				pm.insert(
					prefix + TABLE_COL_GEOMETRY,
					static_pointer_cast<geos::geom::Geometry, LineString>(getGeometry())
				);
			}
			else
			{
				pm.insert(
					prefix + TABLE_COL_GEOMETRY,
					string()
				);
			}
		}



		synthese::SubObjects MainRoadChunk::getSubObjects() const
		{
			SubObjects result;
			return result;
		}



		synthese::LinkedObjectsIds MainRoadChunk::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;
			return result;
		}



		void MainRoadChunk::unlink()
		{
			if(getMainRoadPart())
			{
				getMainRoadPart()->removeEdge(*this);
			}

			// Useful transfer calculation
			if(getHub())
			{
				getHub()->clearAndPropagateUsefulTransfer(RoadModule::GRAPH_ID);
			}

			RoadChunk::unlink();
		}
}	}
