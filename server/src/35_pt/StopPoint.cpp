
/** StopPoint class implementation.
	@file StopPoint.cpp

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

#include "StopPoint.hpp"

#include "Crossing.h"
#include "DataSourceLinksField.hpp"
#include "DRTArea.hpp"
#include "ImportableTableSync.hpp"
#include "Registry.h"
#include "PTModule.h"
#include "PTUseRuleTableSync.h"
#include "RoadChunkTableSync.h"
#include "StopAreaTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "LineStop.h"
#include "JourneyPattern.hpp"
#include "TransportNetwork.h"
#include "RoadChunk.h"

#include <boost/date_time/time_duration.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace impex;
	using namespace road;
	using namespace pt;

	CLASS_DEFINITION(StopPoint, "t012_physical_stops", 12)
	FIELD_DEFINITION_OF_OBJECT(StopPoint, "physical_stop_id", "physical_stop_ids")

	FIELD_DEFINITION_OF_TYPE(ConnectionPlace, "place_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DeprecatedX, "x", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(DeprecatedY, "y", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(OperatorCode, "operator_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ProjectedRoadChunk, "projected_road_chunk_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ProjectedMetricOffset, "projected_metric_offset", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(HandicappedCompliance, "handicapped_compliance_id", SQL_INTEGER)

	namespace pt
	{
		typedef boost::tuple<RegistryKeyType, RegistryKeyType> LineTuple;
		typedef map<string, LineTuple> LinesMap;

		const string StopPoint::DATA_OPERATOR_CODE = "operatorCode";
		const string StopPoint::DATA_X = "x";
		const string StopPoint::DATA_Y = "y";
		const string StopPoint::TAG_STOP_AREA = "stopArea";

		StopPoint::StopPoint(
			RegistryKeyType id,
			string name,
			const StopArea* place,
			boost::shared_ptr<Point> geometry,
			bool withIndexation
		):	Registrable(id),
			Vertex(place, geometry, withIndexation),
			Object<StopPoint, StopPointSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(Name, name),
					FIELD_VALUE_CONSTRUCTOR(ConnectionPlace, place == NULL ?
												boost::optional<StopArea&>(boost::none) :
												boost::optional<StopArea&>(*const_cast<StopArea*>(place))),
					FIELD_DEFAULT_CONSTRUCTOR(DeprecatedX),
					FIELD_DEFAULT_CONSTRUCTOR(DeprecatedY),
					FIELD_DEFAULT_CONSTRUCTOR(OperatorCode),
					FIELD_DEFAULT_CONSTRUCTOR(ProjectedRoadChunk),
					FIELD_DEFAULT_CONSTRUCTOR(ProjectedMetricOffset),
					FIELD_DEFAULT_CONSTRUCTOR(HandicappedCompliance),
					FIELD_VALUE_CONSTRUCTOR(PointGeometry, geometry)))
		{
		}


		StopPoint::~StopPoint()
		{
			unlink();

			BOOST_FOREACH(const Vertex::Edges::value_type& it, getDepartureEdges())
			{
				it.second->setFromVertex(NULL);
			}
			BOOST_FOREACH(const Vertex::Edges::value_type& it, getArrivalEdges())
			{
				it.second->setFromVertex(NULL);
			}
		}



		const StopArea* StopPoint::getConnectionPlace() const
		{
			return (get<ConnectionPlace>() ? get<ConnectionPlace>().get_ptr() : NULL);
		}



		graph::GraphIdType StopPoint::getGraphType() const
		{
			return PTModule::GRAPH_ID;
		}



		graph::VertexAccess StopPoint::getVertexAccess( const road::Crossing& crossing ) const
		{
			if(_projectedPoint.getRoadChunk())
			{
				if(_projectedPoint.getRoadChunk()->getFromCrossing() == &crossing)
				{
					return VertexAccess(
						minutes(static_cast<long>(_projectedPoint.getMetricOffset() / 50)),
						_projectedPoint.getMetricOffset()
					);
				}

				if(_projectedPoint.getRoadChunk()->getForwardEdge().getNext() &&
					static_cast<const Crossing*>(_projectedPoint.getRoadChunk()->getForwardEdge().getNext()->getFromVertex()) == &crossing
				){
					// road chunk metric offsets are expressed from the start of the road
					MetricOffset chunkStartOffset = _projectedPoint.getRoadChunk()->getMetricOffset();
					MetricOffset chunkEndOffset   = _projectedPoint.getRoadChunk()->getForwardEdge().getEndMetricOffset();

					// the metric offset of the projected point is expressed from the start of the chunk
					MetricOffset distanceFromChunkStart = _projectedPoint.getMetricOffset();

					// the distance from the projected point to the chunk end is : chunk size - offset of projected point
					MetricOffset distanceFromChunkEnd   = (chunkStartOffset - chunkEndOffset) - distanceFromChunkStart;

					return VertexAccess(
						minutes(static_cast<long>(distanceFromChunkEnd / 50)),
						distanceFromChunkEnd
					);
				}
			}
			return VertexAccess();
		}



		std::string StopPoint::getRuleUserName() const
		{
			return "Point d'arrêt";
		}



		StopPoint::LinesSet StopPoint::getCommercialLines(
			bool withDepartures,
			bool withArrivals
		) const	{

			// Declarations
			LinesSet lines;

			// Departures
			if(withDepartures)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, getDepartureEdges())
				{
					if(!dynamic_cast<JourneyPattern*>(edge.second->getParentPath()))
					{
						continue;
					}
					lines.insert(
						dynamic_cast<JourneyPattern*>(edge.second->getParentPath())->getCommercialLine()
					);
			}	}

			// Arrivals
			if(withArrivals)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, getArrivalEdges())
				{
					if(!dynamic_cast<JourneyPattern*>(edge.second->getParentPath()))
					{
						continue;
					}
					lines.insert(
						dynamic_cast<JourneyPattern*>(edge.second->getParentPath())->getCommercialLine()
					);
			}	}

			// Exit
			return lines;
		}



		StopPoint::JourneyPatternsMap StopPoint::getJourneyPatterns(
			bool withDepartures /*= true*/,
			bool withArrivals /*= true */
		) const	{

			// Declarations
			JourneyPatternsMap journeyPatterns;

			// Departures
			if(withDepartures)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, getDepartureEdges())
				{
					JourneyPattern* jp(dynamic_cast<JourneyPattern*>(edge.second->getParentPath()));
					if(!jp)
					{
						continue;
					}
					journeyPatterns.insert(
						make_pair(
							jp,
							make_pair(false, true)
					)	);
			}	}

			// Arrivals
			if(withArrivals)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, getArrivalEdges())
				{
					JourneyPattern* jp(dynamic_cast<JourneyPattern*>(edge.second->getParentPath()));
					if(!jp)
					{
						continue;
					}
					JourneyPatternsMap::iterator it(journeyPatterns.find(jp));
					if(it == journeyPatterns.end())
					{
						journeyPatterns.insert(
							make_pair(
								jp,
								make_pair(true, false)
						)	);
					}
					else
					{
						it->second.first = true;
					}
			}	}

			// Exit
			return journeyPatterns;
		}



		void StopPoint::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters /*= true*/,
			const CoordinatesSystem& coordinatesSystem /*= CoordinatesSystem::GetInstanceCoordinatesSystem()*/,
			std::string prefix /*= std::string() */
		) const	{

			pm.insert(prefix + Key::FIELD.name, getKey());
			pm.insert(prefix + Name::FIELD.name, getName());

			// Stop area
			pm.insert(
				prefix + ConnectionPlace::FIELD.name,
				(	dynamic_cast<const StopArea*>(getHub()) ?
					dynamic_cast<const StopArea*>(getHub())->getKey() :
					RegistryKeyType(0)
			)	);

			// Commercial lines
			LinesMap linesMap;
			boost::shared_ptr<ParametersMap> linesPm(new ParametersMap);
			BOOST_FOREACH(const Vertex::Edges::value_type& edge, getDepartureEdges())
			{
				JourneyPattern* jp(dynamic_cast<JourneyPattern*>(edge.second->getParentPath()));
				if(!jp)
				{
					continue;
				}
				linesMap.insert(pair<string, LineTuple>(jp->getCommercialLine()->getShortName(),
					LineTuple(jp->getCommercialLine()->getKey(),
					jp->getCommercialLine()->getNetwork()->getKey()))
				);
			}
			BOOST_FOREACH(const Vertex::Edges::value_type& edge, getArrivalEdges())
			{
				JourneyPattern* jp(dynamic_cast<JourneyPattern*>(edge.second->getParentPath()));
				if(!jp)
				{
					continue;
				}
				linesMap.insert(pair<string, LineTuple>(jp->getCommercialLine()->getShortName(),
					LineTuple(jp->getCommercialLine()->getKey(),
					jp->getCommercialLine()->getNetwork()->getKey()))
				);
			}
			BOOST_FOREACH(const LinesMap::value_type& item, linesMap)
			{
				boost::shared_ptr<ParametersMap> linePm(new ParametersMap);
				linePm->insert(prefix + "line_id", item.second.get<0>());
				linePm->insert(prefix + ShortName::FIELD.name,item.first);
				linePm->insert(prefix + Network::FIELD.name,item.second.get<1>());
				linesPm->insert(prefix + "line",linePm);
			}
			pm.insert(prefix + "lines", linesPm);

			// X Y (deprecated)
			if(hasGeometry())
			{
				pm.insert(
					prefix + DeprecatedX::FIELD.name,
					getGeometry()->getX()
				);
				pm.insert(
					prefix + DeprecatedY::FIELD.name,
					getGeometry()->getY()
				);
			}
			else
			{
				pm.insert(
					prefix + DeprecatedX::FIELD.name,
					string()
				);
				pm.insert(
					prefix + DeprecatedY::FIELD.name,
					string()
				);
			}

			// Projected point
			if(getProjectedPoint().getRoadChunk())
			{
				pm.insert(
					prefix + ProjectedRoadChunk::FIELD.name,
					getProjectedPoint().getRoadChunk()->getKey()
				);
				pm.insert(
					prefix + ProjectedMetricOffset::FIELD.name,
					getProjectedPoint().getMetricOffset()
				);
			}
			else
			{
				pm.insert(
					prefix + ProjectedRoadChunk::FIELD.name,
					string()
				);
				pm.insert(
					prefix + ProjectedMetricOffset::FIELD.name,
					string()
				);
			}

			// Handicapped compliance
			pm.insert(
				prefix + HandicappedCompliance::FIELD.name,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);

			// Geometry
			if(hasGeometry())
			{
				geos::io::WKTWriter writer;
				pm.insert(
					prefix + PointGeometry::FIELD.name,
					writer.write(static_pointer_cast<geos::geom::Geometry, Point>(getGeometry()).get())
				);
			}
			else
			{
				pm.insert(
					prefix + PointGeometry::FIELD.name,
					string()
				);
			}

			pm.insert(
				prefix + ConnectionPlace::FIELD.name,
				(	dynamic_cast<const StopArea*>(getHub()) ?
					dynamic_cast<const StopArea*>(getHub())->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + OperatorCode::FIELD.name,
				impex::DataSourceLinks::Serialize(getDataSourceLinks())
			);

			// Projected point
			if(getProjectedPoint().getRoadChunk())
			{
				pm.insert(
					prefix + ProjectedRoadChunk::FIELD.name,
					getProjectedPoint().getRoadChunk()->getKey()
				);
				pm.insert(
					prefix + ProjectedMetricOffset::FIELD.name,
					getProjectedPoint().getMetricOffset()
				);
			}
			else
			{
				pm.insert(
					prefix + ProjectedRoadChunk::FIELD.name,
					0
				);
				pm.insert(
					prefix + ProjectedMetricOffset::FIELD.name,
					0
				);
			}

			// Handicapped compliance
			pm.insert(
				prefix + HandicappedCompliance::FIELD.name,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);

			// Geometry
			if(hasGeometry())
			{
				boost::shared_ptr<geos::geom::Geometry> projected(getGeometry());
				if(	CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
					static_cast<CoordinatesSystem::SRID>(getGeometry()->getSRID())
				){
					projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*getGeometry());
				}

				geos::io::WKTWriter writer;
				pm.insert(
					prefix + PointGeometry::FIELD.name,
					writer.write(projected.get())
				);
			}
			else
			{
				pm.insert(prefix + PointGeometry::FIELD.name, string());
			}



			pm.insert(prefix + DATA_OPERATOR_CODE, getCodeBySources());
			if(getGeometry().get())
			{
				boost::shared_ptr<Point> gp = coordinatesSystem.convertPoint(*getGeometry());
				if(gp.get())
				{
					pm.insert(prefix + DATA_X, gp->getX());
					pm.insert(prefix + DATA_Y, gp->getY());
				}
			}

			// Stop area data
			if(withAdditionalParameters)
			{
				boost::shared_ptr<ParametersMap> stopAreaPM(new ParametersMap);
				getConnectionPlace()->toParametersMap(*stopAreaPM, &coordinatesSystem);
				pm.insert(TAG_STOP_AREA, stopAreaPM);

				// Extended data source links export
				dataSourceLinksToParametersMap(pm);
			}
		}



		void StopPoint::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			toParametersMap(
				pm,
				withAdditionalParameters,
				CoordinatesSystem::GetInstanceCoordinatesSystem(),
				prefix
			);
		}

		synthese::LinkedObjectsIds StopPoint::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}


		void StopPoint::adaptDeprecatedGeometryIfNecessary()
		{
			// Position : Lon/lat prior to x/y
			if (get<PointGeometry>()) return;
			if ((get<DeprecatedX>() == 0) && (get<DeprecatedY>() == 0)) return;

			boost::shared_ptr<Point> value =
					CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(get<DeprecatedX>(), get<DeprecatedY>());
			set<PointGeometry>(value);
		}


		void StopPoint::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			adaptDeprecatedGeometryIfNecessary();

			// TODO : connection place member field is redundant with Vertex _hub. Vertex should become
			// a pure interface.
			const pt::StopArea* connectionPlace = getConnectionPlace();
			if (connectionPlace != NULL)
			{
				setHub(connectionPlace);
				const_cast<StopArea*>(connectionPlace)->addPhysicalStop(*this);
			}

			// Projected point
			if(get<ProjectedRoadChunk>() && get<ProjectedMetricOffset>())
			{
				if ((_projectedPoint.getRoadChunk() != get<ProjectedRoadChunk>().get_ptr()) ||
					(_projectedPoint.getMetricOffset() != get<ProjectedMetricOffset>()))
				{
					setProjectedPoint(Address(get<ProjectedRoadChunk>().get(), get<ProjectedMetricOffset>()));
				}
			}
			else
			{
					setProjectedPoint(Address());
			}

			RuleUser::Rules rules(getRules());
			if(get<HandicappedCompliance>())
			{
				rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = get<HandicappedCompliance>().get_ptr();
			}
			if(rules != getRules())
			{
				setRules(rules);
			}

			Importable::DataSourceLinks value(
				ImportableTableSync::GetDataSourceLinksFromSerializedString(get<OperatorCode>(), env));
			if(value != getDataSourceLinks())
			{
				setDataSourceLinksWithRegistration(value);
			}

			if(getProjectedPoint().getRoadChunk() &&
				getProjectedPoint().getRoadChunk()->getFromCrossing())
			{
				getProjectedPoint().getRoadChunk()->getFromCrossing()->addReachableVertex(this);
				if(getProjectedPoint().getRoadChunk()->getForwardEdge().getNext())
				{
					static_cast<Crossing*>(getProjectedPoint().getRoadChunk()->getForwardEdge().getNext()->getFromVertex())->addReachableVertex(this);
				}
			}
		}



		void StopPoint::unlink()
		{
			if(getConnectionPlace())
			{
				const_cast<StopArea*>(getConnectionPlace())->removePhysicalStop(*this);
			}
			if(	getProjectedPoint().getRoadChunk() &&
				getProjectedPoint().getRoadChunk()->getFromCrossing()
			){
				getProjectedPoint().getRoadChunk()->getFromCrossing()->removeReachableVertex(this);
			}
			else if(getProjectedPoint().getRoadChunk() &&
					getProjectedPoint().getRoadChunk()->getForwardEdge().getFromVertex()
			){
				static_cast<Crossing*>(getProjectedPoint().getRoadChunk()->getForwardEdge().getFromVertex())->removeReachableVertex(this);
			}
		}


		std::string StopPoint::getName() const
		{
			return get<Name>();
		}


		void StopPoint::setName(const std::string& value)
		{
			set<Name>(value);
		}


		bool StopPoint::allowUpdate(const server::Session* session) const
		{
			return true;
		}

		bool StopPoint::allowCreate(const server::Session* session) const
		{
			return true;
		}

		bool StopPoint::allowDelete(const server::Session* session) const
		{
			return true;
		}

}	}
