
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
	using namespace road;

	CLASS_DEFINITION(RoadChunk, "t014_road_chunks", 14)
	FIELD_DEFINITION_OF_OBJECT(RoadChunk, "road_chunk_id", "road_chunk_ids")

	FIELD_DEFINITION_OF_TYPE(FromCrossing, "address_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RankInPathField, "rank_in_path", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MetricOffsetField, "metric_offset", SQL_DOUBLE)

	FIELD_DEFINITION_OF_TYPE(LeftStartHouseNumber, "left_start_house_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LeftEndHouseNumber, "left_end_house_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RightStartHouseNumber, "right_start_house_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RightEndHouseNumber, "right_end_house_number", SQL_INTEGER)

	FIELD_DEFINITION_OF_TYPE(LeftHouseNumberingPolicy, "left_house_numbering_policy", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RightHouseNumberingPolicy, "right_house_numbering_policy", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(OneWay, "one_way", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CarSpeed, "car_speed", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(NonWalkable, "non_walkable", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(NonDrivable, "non_drivable", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(NonBikable, "non_bikable", SQL_BOOLEAN)

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
			Object<RoadChunk, RoadChunkSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(FromCrossing, fromCrossing == NULL ?
												boost::optional<Crossing&>(boost::none) :
												boost::optional<Crossing&>(*fromCrossing)),
					FIELD_VALUE_CONSTRUCTOR(RankInPathField, rankInRoad),
					FIELD_VALUE_CONSTRUCTOR(Road, boost::optional<Road&>(*street)),
					FIELD_VALUE_CONSTRUCTOR(MetricOffsetField, metricOffset),
					FIELD_DEFAULT_CONSTRUCTOR(LeftStartHouseNumber),
					FIELD_DEFAULT_CONSTRUCTOR(LeftEndHouseNumber),
					FIELD_DEFAULT_CONSTRUCTOR(RightStartHouseNumber),
					FIELD_DEFAULT_CONSTRUCTOR(RightEndHouseNumber),
					FIELD_VALUE_CONSTRUCTOR(LeftHouseNumberingPolicy, ALL_NUMBERS),
					FIELD_VALUE_CONSTRUCTOR(RightHouseNumberingPolicy, ALL_NUMBERS),
					FIELD_DEFAULT_CONSTRUCTOR(OneWay),
					FIELD_VALUE_CONSTRUCTOR(CarSpeed, carSpeed),
					FIELD_VALUE_CONSTRUCTOR(NonWalkable, false),
					FIELD_VALUE_CONSTRUCTOR(NonDrivable, false),
					FIELD_VALUE_CONSTRUCTOR(NonBikable, false),
					FIELD_DEFAULT_CONSTRUCTOR(LineStringGeometry)
			)),
			_forwardEdge(new RoadChunkEdge(*this, false)),
			_reverseEdge(new RoadChunkEdge(*this, true))
		{
			_updateEdges();
		}


		RoadChunk::RoadChunk(util::RegistryKeyType id)
			: util::Registrable(id),
			  Object<RoadChunk, RoadChunkSchema>(
				  Schema(
					  FIELD_VALUE_CONSTRUCTOR(Key, id),
					  FIELD_DEFAULT_CONSTRUCTOR(FromCrossing),
					  FIELD_DEFAULT_CONSTRUCTOR(RankInPathField),
					  FIELD_DEFAULT_CONSTRUCTOR(Road),
					  FIELD_DEFAULT_CONSTRUCTOR(MetricOffsetField),
					  FIELD_DEFAULT_CONSTRUCTOR(LeftStartHouseNumber),
					  FIELD_DEFAULT_CONSTRUCTOR(LeftEndHouseNumber),
					  FIELD_DEFAULT_CONSTRUCTOR(RightStartHouseNumber),
					  FIELD_DEFAULT_CONSTRUCTOR(RightEndHouseNumber),
					  FIELD_VALUE_CONSTRUCTOR(LeftHouseNumberingPolicy, ALL_NUMBERS),
					  FIELD_VALUE_CONSTRUCTOR(RightHouseNumberingPolicy, ALL_NUMBERS),
					  FIELD_DEFAULT_CONSTRUCTOR(OneWay),
					  FIELD_DEFAULT_CONSTRUCTOR(CarSpeed),
					  FIELD_VALUE_CONSTRUCTOR(NonWalkable, false),
					  FIELD_VALUE_CONSTRUCTOR(NonDrivable, false),
					  FIELD_VALUE_CONSTRUCTOR(NonBikable, false),
					  FIELD_DEFAULT_CONSTRUCTOR(LineStringGeometry)
			  )),
			  _forwardEdge(new RoadChunkEdge(*this, false)),
			  _reverseEdge(new RoadChunkEdge(*this, true))
		{
			_updateEdges();
		}


		void RoadChunk::_updateEdges() const
		{
			Road* road(getRoad());
			// Road
			if(road)
			{
				_forwardEdge->_parentPath = &road->getForwardPath();
				_reverseEdge->_parentPath = &road->getReversePath();
			}
			else
			{
				_forwardEdge->_parentPath = NULL;
				_reverseEdge->_parentPath = NULL;
			}

			// Crossing
			_forwardEdge->_fromVertex = getFromCrossing();
			_reverseEdge->_fromVertex = getFromCrossing();

			// Metric offset
			_forwardEdge->setMetricOffset(get<MetricOffsetField>());
			_reverseEdge->setMetricOffset(-get<MetricOffsetField>());

			// Rank
			_forwardEdge->setRankInPath(get<RankInPathField>());
			_reverseEdge->setRankInPath(numeric_limits<size_t>::max() - get<RankInPathField>());

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
			if(get<NonWalkable>())
			{
				rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
				rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}				

			if(get<NonBikable>())
			{
				rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}				

			if(get<NonDrivable>() || noMotorVehicles || get<OneWay>() == -1)
			{
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}
			_forwardEdge->setRules(rules);

			if(get<OneWay>() == 1)
			{
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			}
			else if(get<OneWay>() == -1 && !noMotorVehicles && !get<NonDrivable>())
			{
				rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			}
			_reverseEdge->setRules(rules);
		}



		RoadChunk::~RoadChunk ()
		{
		}



		double RoadChunk::getCarSpeed(bool nominalSpeed) const
		{
			if(nominalSpeed || !this->getRoad())
			{
				return get<CarSpeed>();
			}
			else
			{
				if(CAR_SPEED_FACTORS._factors.find(this->getRoad()->get<RoadTypeField>()) != CAR_SPEED_FACTORS._factors.end())
				{
					return get<CarSpeed>() * CAR_SPEED_FACTORS._factors.find(this->getRoad()->get<RoadTypeField>())->second;
				}
				return get<CarSpeed>();
			}
		}



		boost::shared_ptr<geos::geom::Point> RoadChunk::getPointFromOffset(
			MetricOffset metricOffset
		) const	{
			boost::shared_ptr<LineString> geometry(_forwardEdge->getRealGeometry());
			double offset = metricOffset - _forwardEdge->getMetricOffset();
			if(!geometry.get() || geometry->isEmpty())
			{
				Crossing* crossing(getFromCrossing());
				if (crossing)
				{
					return crossing->getGeometry();
				}
				else
				{
					return boost::shared_ptr<geos::geom::Point>();
				}
			}
			if(offset > geometry->getLength())
			{
				return boost::shared_ptr<Point>(geometry->getEndPoint());
			}
			return boost::shared_ptr<Point>(
				geometry->getFactory()->createPoint(
					LengthIndexedLine(geometry.get()).extractPoint(offset)
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
				_testIfHouseNumberBelongsToBounds(getLeftHouseNumberBounds(), getLeftHouseNumberingPolicy(), houseNumber) ||
				_testIfHouseNumberBelongsToBounds(getRightHouseNumberBounds(), getRightHouseNumberingPolicy(), houseNumber) ||
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

			return
				startOffset + (endOffset - startOffset) * (
					(bounds->first < bounds->second) ?
					((double)(houseNumber - (bounds->first - 1)) / (double)((bounds->second + 1) - (bounds->first - 1))) :
					((double)((bounds->first + 1) - houseNumber) / (double)((bounds->first + 1) - (bounds->second - 1)))
				)
			;
		}



		MetricOffset RoadChunk::getHouseNumberMetricOffset(
			HouseNumber houseNumber
		) const	{

			// Left side
			if(	_testIfHouseNumberBelongsToBounds(getLeftHouseNumberBounds(), getLeftHouseNumberingPolicy(), houseNumber))
			{
				return _computeMetricOffset(getLeftHouseNumberBounds(), houseNumber, getMetricOffset(), _forwardEdge->getEndMetricOffset());
			}

			// Right side
			if(	_testIfHouseNumberBelongsToBounds(getRightHouseNumberBounds(), getRightHouseNumberingPolicy(), houseNumber))
			{
				return _computeMetricOffset(getRightHouseNumberBounds(), houseNumber, getMetricOffset(), _forwardEdge->getEndMetricOffset());
			}

			return getMetricOffset();
		}



		HouseNumber RoadChunk::getHouseNumberFromOffset(
			double metricOffset
		) const {
			double relativePosition = (metricOffset - this->getMetricOffset()) / (this->getForwardEdge().getEndMetricOffset() - this->getMetricOffset());
			HouseNumberBounds bounds = getLeftHouseNumberBounds();

			if(!bounds)
			{
				return 0;
			}
			else if(relativePosition > 1)
			{
				return bounds->second;
			}
			else if(relativePosition < 0)
			{
				return bounds->first;
			}
			else
			{
				HouseNumber closestHouseNumber(
					(bounds->first <= bounds->second) ?
						min(
							(double)bounds->second,
							ceil((relativePosition * ((bounds->second + 1) - (bounds->first - 1))) + (bounds->first - 1))
						) :
						max(
							(double)bounds->first,
							floor((bounds->first + 1) - (relativePosition * ((bounds->first + 1) - (bounds->second - 1))))
						)
				);

				switch(getLeftHouseNumberingPolicy())
				{
					case ODD_NUMBERS:
						return closestHouseNumber - (closestHouseNumber % 2) + 1;
					case EVEN_NUMBERS:
						return closestHouseNumber - (closestHouseNumber % 2);
					case ALL_NUMBERS:
					default:
						return closestHouseNumber;
				}
			}
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


		void RoadChunk::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			
			pm.insert(prefix + Key::FIELD.name, getKey());
			pm.insert(prefix + FromCrossing::FIELD.name, getFromCrossing() ? getFromCrossing()->getKey() : RegistryKeyType(0));
			pm.insert(prefix + RankInPathField::FIELD.name, get<RankInPathField>());
			pm.insert(prefix + Road::FIELD.name, getRoad() ? getRoad()->getKey() : RegistryKeyType(0));
			pm.insert(prefix + MetricOffsetField::FIELD.name, get<MetricOffsetField>());
			pm.insert(prefix + LeftStartHouseNumber::FIELD.name, getLeftHouseNumberBounds() ? lexical_cast<string>(getLeftHouseNumberBounds()->first) : string());
			pm.insert(prefix + LeftEndHouseNumber::FIELD.name, getLeftHouseNumberBounds() ? lexical_cast<string>(getLeftHouseNumberBounds()->second) : string());
			pm.insert(prefix + RightStartHouseNumber::FIELD.name, getRightHouseNumberBounds() ? lexical_cast<string>(getRightHouseNumberBounds()->first) : string());
			pm.insert(prefix + RightEndHouseNumber::FIELD.name, getRightHouseNumberBounds() ? lexical_cast<string>(getRightHouseNumberBounds()->second) : string());
			pm.insert(prefix + LeftHouseNumberingPolicy::FIELD.name, static_cast<int>(getLeftHouseNumberBounds() ? getLeftHouseNumberingPolicy() : ALL_NUMBERS));
			pm.insert(prefix + RightHouseNumberingPolicy::FIELD.name, static_cast<int>(getRightHouseNumberBounds() ? getRightHouseNumberingPolicy() : ALL_NUMBERS));
			pm.insert(prefix + OneWay::FIELD.name, get<OneWay>());
			pm.insert(prefix + CarSpeed::FIELD.name, getCarSpeed(true));
			pm.insert(prefix + NonWalkable::FIELD.name, get<NonWalkable>());
			pm.insert(prefix + NonDrivable::FIELD.name, get<NonDrivable>());
			pm.insert(prefix + NonBikable::FIELD.name, get<NonBikable>());

			if(hasGeometry())
			{
				if(!getGeometry().get() || getGeometry()->isEmpty())
				{
					pm.insert(
						prefix + LineStringGeometry::FIELD.name,
						string()
					);
				}
				else
				{
					boost::shared_ptr<geos::geom::Geometry> projected(getGeometry());
					if(	CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
						static_cast<CoordinatesSystem::SRID>(getGeometry()->getSRID())
					){
						projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*getGeometry());
					}
					
					geos::io::WKTWriter writer;
					pm.insert(
						prefix + LineStringGeometry::FIELD.name,
						writer.write(projected.get())
					);
				}
			}
			else
			{
				pm.insert(
					prefix + LineStringGeometry::FIELD.name,
					string()
				);
			}
		}



		synthese::SubObjects RoadChunk::getSubObjects() const
		{
			SubObjects result;
			return result;
		}


		void RoadChunk::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			_updateEdges();

			setGeometry(get<LineStringGeometry>());

			_forwardEdge->link();
			_reverseEdge->link();
		}


		void RoadChunk::unlink()
		{
			_forwardEdge->unlink();
			_reverseEdge->unlink();
		}


		bool RoadChunk::allowUpdate(const server::Session* session) const
		{
			return true;
		}

		bool RoadChunk::allowCreate(const server::Session* session) const
		{
			return true;
		}

		bool RoadChunk::allowDelete(const server::Session* session) const
		{
			return true;
		}

		boost::shared_ptr<geos::geom::LineString> RoadChunk::getRealGeometry() const
		{
			return _forwardEdge->getRealGeometry();
		}


		Road*
		RoadChunk::getRoad() const
		{
			return (get<Road>() ? get<Road>().get_ptr() : NULL);
		}

		Crossing*
		RoadChunk::getFromCrossing() const
		{
			return (get<FromCrossing>() ? get<FromCrossing>().get_ptr() : NULL);
		}

		graph::MetricOffset
		RoadChunk::getMetricOffset() const
		{
			return get<MetricOffsetField>();
		}


		const HouseNumberingPolicy&
		RoadChunk::getLeftHouseNumberingPolicy() const
		{
			return get<LeftHouseNumberingPolicy>();
		}


		const HouseNumberingPolicy&
		RoadChunk::getRightHouseNumberingPolicy() const
		{
			return get<RightHouseNumberingPolicy>();
		}

		HouseNumberBounds
		RoadChunk::getLeftHouseNumberBounds() const
		{
			if(get<LeftStartHouseNumber>())
			{
				return boost::make_optional(
					std::make_pair(
						(HouseNumber) get<LeftStartHouseNumber>(),
						(HouseNumber) get<LeftEndHouseNumber>()
					)
				);
			}

			return HouseNumberBounds();
		}

		HouseNumberBounds
		RoadChunk::getRightHouseNumberBounds() const
		{
			if(get<RightStartHouseNumber>())
			{
				return boost::make_optional(
					std::make_pair(
						(HouseNumber) get<RightStartHouseNumber>(),
						(HouseNumber) get<RightEndHouseNumber>()
					)
				);
			}

			return HouseNumberBounds();
		}

		size_t
		RoadChunk::getRankInPath() const
		{
			return get<RankInPathField>();
		}

		void
		RoadChunk::setRankInPath(size_t value)
		{
			set<RankInPathField>(value);
		}

		void
		RoadChunk::setRoad(Road* road)
		{
			set<Road>(road
						? boost::optional<Road&>(*const_cast<Road*>(road))
						: boost::none);
		}


		void
		RoadChunk::setFromCrossing(Crossing* crossing)
		{
			set<FromCrossing>(crossing
						? boost::optional<Crossing&>(*const_cast<Crossing*>(crossing))
						: boost::none);
		}


		void
		RoadChunk::setMetricOffset(graph::MetricOffset value)
		{
			set<MetricOffsetField>(value);
		}

		void
		RoadChunk::setCarOneWay(int value)
		{
			set<OneWay>(value);
		}

		void RoadChunk::setCarSpeed(double carSpeed)
		{
			set<CarSpeed>(carSpeed);
		}

		void RoadChunk::setNonWalkable(bool value)
		{
			set<NonWalkable>(value);
		}

		void RoadChunk::setNonDrivable(bool value)
		{
			set<NonDrivable>(value);
		}

		void RoadChunk::setNonBikable(bool value)
		{
			set<NonBikable>(value);
		}

		void RoadChunk::setLeftHouseNumberBounds(const HouseNumberBounds& value)
		{
			set<LeftStartHouseNumber>(value.get().first);
			set<LeftEndHouseNumber>(value.get().second);
		}

		void RoadChunk::setRightHouseNumberBounds(const HouseNumberBounds& value)
		{
			set<RightStartHouseNumber>(value.get().first);
			set<RightEndHouseNumber>(value.get().second);
		}

		void RoadChunk::setLeftHouseNumberingPolicy(const HouseNumberingPolicy& value)
		{
			set<LeftHouseNumberingPolicy>(value);
		}

		void RoadChunk::setRightHouseNumberingPolicy(const HouseNumberingPolicy& value)
		{
			set<RightHouseNumberingPolicy>(value);
		}


}	}
