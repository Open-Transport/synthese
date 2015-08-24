
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
#include "StopPointTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "LineStop.h"
#include "JourneyPattern.hpp"
#include "TransportNetwork.h"

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
	

	namespace util
	{
		template<> const string Registry<StopPoint>::KEY("StopPoint");
	}

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
			_name(name)
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
			return static_cast<const StopArea*>(Vertex::getHub());
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

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(prefix + StopPointTableSync::COL_NAME, getName());

			// Stop area
			pm.insert(
				prefix + StopPointTableSync::COL_PLACEID,
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
				linePm->insert(prefix + CommercialLineTableSync::COL_SHORT_NAME,item.first);
				linePm->insert(prefix + CommercialLineTableSync::COL_NETWORK_ID,item.second.get<1>());
				linesPm->insert(prefix + "line",linePm);
			}
			pm.insert(prefix + "lines", linesPm);

			// X Y (deprecated)
			if(hasGeometry())
			{
				pm.insert(
					prefix + StopPointTableSync::COL_X,
					getGeometry()->getX()
				);
				pm.insert(
					prefix + StopPointTableSync::COL_Y,
					getGeometry()->getY()
				);
			}
			else
			{
				pm.insert(
					prefix + StopPointTableSync::COL_X,
					string()
				);
				pm.insert(
					prefix + StopPointTableSync::COL_Y,
					string()
				);
			}

			// Projected point
			if(getProjectedPoint().getRoadChunk())
			{
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID,
					getProjectedPoint().getRoadChunk()->getKey()
				);
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_METRIC_OFFSET,
					getProjectedPoint().getMetricOffset()
				);
			}
			else
			{
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID,
					string()
				);
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_METRIC_OFFSET,
					string()
				);
			}

			// Handicapped compliance
			pm.insert(
				prefix + StopPointTableSync::COL_HANDICAPPED_COMPLIANCE_ID,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);

			// Geometry
			if(hasGeometry())
			{
				geos::io::WKTWriter writer;
				pm.insert(
					prefix + TABLE_COL_GEOMETRY,
					writer.write(static_pointer_cast<geos::geom::Geometry, Point>(getGeometry()).get())
				);
			}
			else
			{
				pm.insert(
					prefix + TABLE_COL_GEOMETRY,
					string()
				);
			}

			pm.insert(
				prefix + StopPointTableSync::COL_PLACEID,
				(	dynamic_cast<const StopArea*>(getHub()) ?
					dynamic_cast<const StopArea*>(getHub())->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + StopPointTableSync::COL_OPERATOR_CODE,
				impex::DataSourceLinks::Serialize(getDataSourceLinks())
			);

			// Projected point
			if(getProjectedPoint().getRoadChunk())
			{
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID, 
					getProjectedPoint().getRoadChunk()->getKey()
				);
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_METRIC_OFFSET,
					getProjectedPoint().getMetricOffset()
				);
			}
			else
			{
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID, 
					0
				);
				pm.insert(
					prefix + StopPointTableSync::COL_PROJECTED_METRIC_OFFSET,
					0
				);
			}

			// Handicapped compliance
			pm.insert(
				prefix + StopPointTableSync::COL_HANDICAPPED_COMPLIANCE_ID,
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
					prefix + TABLE_COL_GEOMETRY,
					writer.write(projected.get())
				);
			}
			else
			{
				pm.insert(prefix + TABLE_COL_GEOMETRY, string());
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

		bool StopPoint::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Name
			if(record.isDefined(StopPointTableSync::COL_NAME))
			{
				string value(record.get<string>(StopPointTableSync::COL_NAME));
				if(value != getName())
				{
					setName(value);
					result = true;
				}
			}

			// Position : Lon/lat prior to x/y
			if(	record.isDefined(TABLE_COL_GEOMETRY) ||
				(record.isDefined(StopPointTableSync::COL_X) && record.isDefined(StopPointTableSync::COL_Y))
			){
				boost::shared_ptr<Point> value;
				if(!record.get<string>(TABLE_COL_GEOMETRY).empty())
				{
					value = static_pointer_cast<Point, geos::geom::Geometry>(
						record.getGeometryFromWKT(TABLE_COL_GEOMETRY)
					);
				}
				else if(
					record.getDefault<double>(StopPointTableSync::COL_X, 0) > 0 &&
					record.getDefault<double>(StopPointTableSync::COL_Y, 0) > 0
				){
					value = CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
						record.getDefault<double>(StopPointTableSync::COL_X, 0),
						record.getDefault<double>(StopPointTableSync::COL_Y, 0)
					);
				}
				if( (!value.get() && getGeometry().get()) ||
					(value.get() && !getGeometry().get()) ||
					(value.get() && getGeometry().get() && !value->equalsExact(getGeometry().get(), 0.01))
				){
					setGeometry(value);
					result = true;
				}
			}

//			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
//			{
				// Stop area
			if(record.isDefined(StopPointTableSync::COL_PLACEID))
			{
				StopArea* place(NULL);
				try
				{
					place = StopAreaTableSync::GetEditable(
						record.getDefault<RegistryKeyType>(StopPointTableSync::COL_PLACEID, 0),
						env
					).get();
				}
				catch (ObjectNotFoundException<StopArea>& e)
				{
					throw Exception("Not found");
				}
				if(place != getConnectionPlace())
				{
					setHub(place);
					result = true;
				}
			}

			// Projected point
			if(	record.isDefined(StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID) &&
				record.isDefined(StopPointTableSync::COL_PROJECTED_METRIC_OFFSET)
			){
				RegistryKeyType chunkId(
					record.getDefault<RegistryKeyType>(StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID,0));
				RoadChunk* chunk(NULL);
				MetricOffset metricOffset(0);
				if(chunkId > 0)
				{
					try
					{
						chunk = RoadChunkTableSync::GetEditable(chunkId, env).get();
						metricOffset = record.getDefault<double>(StopPointTableSync::COL_PROJECTED_METRIC_OFFSET, 0);
					}
					catch (ObjectNotFoundException<RoadChunk>&)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(chunkId) + " for projected chunk in stop " + lexical_cast<string>(getKey()));
					}
				}
				if(	getProjectedPoint().getRoadChunk() != chunk ||
					getProjectedPoint().getMetricOffset() != metricOffset
				){
					if(chunk)
					{
						setProjectedPoint(Address(*chunk, metricOffset));
					}
					else
					{
						setProjectedPoint(Address());
					}
					result = true;
				}
			}


				// Handicapped compliance
				if(record.isDefined(StopPointTableSync::COL_HANDICAPPED_COMPLIANCE_ID))
				{
					RuleUser::Rules rules(getRules());
					RegistryKeyType handicappedComplianceId(
						record.getDefault<RegistryKeyType>(StopPointTableSync::COL_HANDICAPPED_COMPLIANCE_ID, 0)
					);
					if(handicappedComplianceId > 0)
					{
						try
						{
							rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(handicappedComplianceId, env).get();
						}
						catch(ObjectNotFoundException<PTUseRule>&)
						{
							Log::GetInstance().warn("Bad value " + lexical_cast<string>(handicappedComplianceId) + " for handicapped compliance in stop " + lexical_cast<string>(getKey()));
					}	}
					if(rules != getRules())
					{
						setRules(rules);
						result = true;
					}
				}

				// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
				if(record.isDefined(StopPointTableSync::COL_OPERATOR_CODE))
				{
					Importable::DataSourceLinks value(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							record.get<string>(StopPointTableSync::COL_OPERATOR_CODE),
							env
					)	);
					if(value != getDataSourceLinks())
					{
						setDataSourceLinksWithRegistration(value);
						result = true;
					}
				}
//			}
			return result;
		}

		synthese::LinkedObjectsIds StopPoint::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}

		void StopPoint::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(getConnectionPlace())
			{
				const_cast<StopArea*>(getConnectionPlace())->addPhysicalStop(*this);
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
}	}
