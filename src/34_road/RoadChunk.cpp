
/** RoadChunk class implementation.
	@file RoadChunk.cpp

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

#include "RoadChunk.h"

#include "AllowedUseRule.h"
#include "CrossingTableSync.hpp"
#include "ForbiddenUseRule.h"
#include "House.hpp"
#include "Road.h"
#include "RoadPath.hpp"
#include "RoadChunkEdge.hpp"
#include "RoadChunkTableSync.h"
#include "RoadTableSync.h"

#include <geos/geom/LineString.h>
#include <geos/linearref/LengthIndexedLine.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::linearref;

namespace synthese
{
	using namespace db;
	using namespace graph;
	using namespace util;	

	namespace util
	{
		template<> const string Registry<road::RoadChunk>::KEY("RoadChunk");
	}

	namespace road
	{
		const CarSpeedFactors RoadChunk::CAR_SPEED_FACTORS = CarSpeedFactors();

		RoadChunk::RoadChunk(
			util::RegistryKeyType id,
			Crossing* fromCrossing,
			size_t rankInRoad,
			Road* street,
			double metricOffset,
			double carSpeed
		):	util::Registrable(id),
			_carOneWay(0),
			_carSpeed(carSpeed),
			_road(street),
			_crossing(fromCrossing),
			_rankInPath(rankInRoad),
			_metricOffset(metricOffset),
			_nonWalkable(false),
			_nonDrivable(false),
			_nonBikable(false),
			_forwardEdge(new RoadChunkEdge(*this, false)),
			_reverseEdge(new RoadChunkEdge(*this, true))
		{
			_updateEdges();
		}



		void RoadChunk::_updateEdges() const
		{
			// Road
			if(_road)
			{
				_forwardEdge->_parentPath = &_road->getForwardPath();
				_reverseEdge->_parentPath = &_road->getReversePath();
			}
			else
			{
				_forwardEdge->_parentPath = NULL;
				_reverseEdge->_parentPath = NULL;
			}

			// Crossing
			_forwardEdge->_fromVertex = _crossing;
			_reverseEdge->_fromVertex = _crossing;

			// Metric offset
			_forwardEdge->setMetricOffset(_metricOffset);
			_reverseEdge->setMetricOffset(-_metricOffset);

			// Rank
			_forwardEdge->setRankInPath(_rankInPath);
			_reverseEdge->setRankInPath(numeric_limits<size_t>::max() - _rankInPath);

			// Use rules
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
				switch(getRoad()->get<RoadTypeField>())
				{
				case ROAD_TYPE_PEDESTRIANSTREET:
				case ROAD_TYPE_PEDESTRIANPATH:
				case ROAD_TYPE_STEPS:
				case ROAD_TYPE_PRIVATEWAY:
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
			if(_nonWalkable)
			{
				rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}				

			if(_nonBikable)
			{
				rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}				

			if(_nonDrivable || noMotorVehicles || getCarOneWay() == -1)
			{
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}
			_forwardEdge->setRules(rules);

			if(getCarOneWay() == 1)
			{
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}
			else if(getCarOneWay() == -1 && !noMotorVehicles && !_nonDrivable)
			{
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			}
			_reverseEdge->setRules(rules);
		}



		RoadChunk::~RoadChunk ()
		{
			unlink();
		}



		double RoadChunk::getCarSpeed(bool nominalSpeed) const
		{
			if(nominalSpeed)
			{
				return _carSpeed;
			}
			else
			{
				if(CAR_SPEED_FACTORS._factors.find(this->getRoad()->get<RoadTypeField>()) != CAR_SPEED_FACTORS._factors.end())
				{
					return _carSpeed * CAR_SPEED_FACTORS._factors.find(this->getRoad()->get<RoadTypeField>())->second;
				}
				return _carSpeed;
			}
		}




		boost::shared_ptr<geos::geom::Point> RoadChunk::getPointFromOffset(
			MetricOffset metricOffset
		) const	{
			boost::shared_ptr<LineString> geometry(_forwardEdge->getRealGeometry());
			if(!geometry.get() || geometry->isEmpty())
			{
				return _crossing->getGeometry();
			}
			if(metricOffset > geometry->getLength())
			{
				return boost::shared_ptr<Point>(geometry->getEndPoint());
			}
			return boost::shared_ptr<Point>(
				geometry->getFactory()->createPoint(
					LengthIndexedLine(geometry.get()).extractPoint(metricOffset)
			)	);
		}



		bool RoadChunk::_testIfHouseNumberBelongsToBounds(
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
			case ALL_NUMBERS: return true;
			case ODD_NUMBERS: return houseNumber % 2;
			case EVEN_NUMBERS: return !(houseNumber % 2);
			}

			// Never execute
			return false;
		}


		bool RoadChunk::testIfHouseNumberBelongsToChunk(
			HouseNumber houseNumber
		) const	{
			return
				_testIfHouseNumberBelongsToBounds(_leftHouseNumberBounds, _leftHouseNumberingPolicy, houseNumber) ||
				_testIfHouseNumberBelongsToBounds(_rightHouseNumberBounds, _rightHouseNumberingPolicy, houseNumber) ||
				_houses.count(houseNumber);
			;
		}


		MetricOffset RoadChunk::_computeMetricOffset(
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



		MetricOffset RoadChunk::getHouseNumberMetricOffset(
			HouseNumber houseNumber
		) const	{

			// Left side
			if(	_testIfHouseNumberBelongsToBounds(_leftHouseNumberBounds, _leftHouseNumberingPolicy, houseNumber))
			{
				return _computeMetricOffset(_leftHouseNumberBounds, houseNumber, getMetricOffset(), _forwardEdge->getEndMetricOffset());
			}

			// Right side
			if(	_testIfHouseNumberBelongsToBounds(_rightHouseNumberBounds, _rightHouseNumberingPolicy, houseNumber))
			{
				return _computeMetricOffset(_rightHouseNumberBounds, houseNumber, getMetricOffset(), _forwardEdge->getEndMetricOffset());
			}

			return getMetricOffset();
		}



		void RoadChunk::addHouse(House& house)
		{
			if(house.getHouseNumber())
			{
				_houses[*house.getHouseNumber()] = &house;
			}
		}



		void RoadChunk::removeHouse(House& house)
		{
			if(house.getHouseNumber())
			{
				_houses.erase(*house.getHouseNumber());
			}
		}



		bool RoadChunk::loadFromRecord( const Record& record, util::Env& env )
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
				boost::shared_ptr<Road> value;
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
				HouseNumberingPolicy policyValue(ALL_NUMBERS);

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
				HouseNumberingPolicy policyValue(ALL_NUMBERS);

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
				bool value(record.getDefault<bool>(RoadChunkTableSync::COL_NON_WALKABLE, false));
				if(value != _nonWalkable)
				{
					_nonWalkable = value;
					result = true;
				}
			}

			// Use rules
			{
				bool value(record.getDefault<bool>(RoadChunkTableSync::COL_NON_DRIVABLE, false));
				if(value != _nonDrivable)
				{
					_nonDrivable = value;
					result = true;
				}
			}

			// Use rules
			{
				bool value(record.getDefault<bool>(RoadChunkTableSync::COL_NON_BIKABLE, false));
				if(value != _nonBikable)
				{
					_nonBikable = value;
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



		void RoadChunk::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			_updateEdges();

			_forwardEdge->link();
			_reverseEdge->link();
		}



		void RoadChunk::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(prefix + RoadChunkTableSync::COL_CROSSING_ID, getFromCrossing() ? getFromCrossing()->getKey() : RegistryKeyType(0));
			pm.insert(prefix + RoadChunkTableSync::COL_RANKINPATH, getRankInPath());
			pm.insert(prefix + RoadChunkTableSync::COL_ROADID, getRoad() ? getRoad()->getKey() : RegistryKeyType(0));
			pm.insert(prefix + RoadChunkTableSync::COL_METRICOFFSET, getMetricOffset());
			pm.insert(prefix + RoadChunkTableSync::COL_LEFT_START_HOUSE_NUMBER, getLeftHouseNumberBounds() ? lexical_cast<string>(getLeftHouseNumberBounds()->first) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_LEFT_END_HOUSE_NUMBER, getLeftHouseNumberBounds() ? lexical_cast<string>(getLeftHouseNumberBounds()->second) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_RIGHT_START_HOUSE_NUMBER, getRightHouseNumberBounds() ? lexical_cast<string>(getRightHouseNumberBounds()->first) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_RIGHT_END_HOUSE_NUMBER, getRightHouseNumberBounds() ? lexical_cast<string>(getRightHouseNumberBounds()->second) : string());
			pm.insert(prefix + RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY, static_cast<int>(getLeftHouseNumberBounds() ? getLeftHouseNumberingPolicy() : ALL_NUMBERS));
			pm.insert(prefix + RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY, static_cast<int>(getRightHouseNumberBounds() ? getRightHouseNumberingPolicy() : ALL_NUMBERS));
			pm.insert(prefix + RoadChunkTableSync::COL_ONE_WAY, getCarOneWay());
			pm.insert(prefix + RoadChunkTableSync::COL_CAR_SPEED, getCarSpeed(true));
			pm.insert(prefix + RoadChunkTableSync::COL_NON_WALKABLE, _nonWalkable);
			pm.insert(prefix + RoadChunkTableSync::COL_NON_DRIVABLE, _nonDrivable);
			pm.insert(prefix + RoadChunkTableSync::COL_NON_BIKABLE, _nonBikable);

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



		synthese::SubObjects RoadChunk::getSubObjects() const
		{
			SubObjects result;
			return result;
		}



		synthese::LinkedObjectsIds RoadChunk::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;
			return result;
		}



		void RoadChunk::unlink()
		{
			_forwardEdge->unlink();
			_reverseEdge->unlink();
		}



		boost::shared_ptr<geos::geom::LineString> RoadChunk::getRealGeometry() const
		{
			return _forwardEdge->getRealGeometry();
		}
}	}
