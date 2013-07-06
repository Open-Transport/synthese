
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
#include "Registry.h"
#include "PTModule.h"
#include "StopArea.hpp"
#include "ReverseRoadChunk.hpp"
#include "LineStop.h"
#include "JourneyPattern.hpp"

#include <boost/date_time/time_duration.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace impex;

	namespace util
	{
		template<> const string Registry<StopPoint>::KEY("StopPoint");
	}

	namespace pt
	{
		const string StopPoint::DATA_ID = "id";
		const string StopPoint::DATA_NAME = "name";
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
			Vertex(place, geometry, withIndexation)
		{
		}



		StopPoint::~StopPoint()
		{

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
					return VertexAccess(minutes(static_cast<long>(_projectedPoint.getMetricOffset() / 50)), _projectedPoint.getMetricOffset());
				}
				if(	_projectedPoint.getRoadChunk()->getReverseRoadChunk() &&
					_projectedPoint.getRoadChunk()->getReverseRoadChunk()->getFromCrossing() == &crossing
				){
					return VertexAccess(
						minutes(static_cast<long>((_projectedPoint.getRoadChunk()->getEndMetricOffset() - _projectedPoint.getRoadChunk()->getMetricOffset() - _projectedPoint.getMetricOffset()) / 50)),
						_projectedPoint.getRoadChunk()->getEndMetricOffset() - _projectedPoint.getRoadChunk()->getMetricOffset() - _projectedPoint.getMetricOffset()
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
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					lines.insert(
						static_cast<const LineStop*>(edge.second)->getLine()->getCommercialLine()
					);
			}	}

			// Arrivals
			if(withArrivals)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, getArrivalEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					lines.insert(
						static_cast<const LineStop*>(edge.second)->getLine()->getCommercialLine()
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
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					journeyPatterns.insert(
						make_pair(
							static_cast<const LineStop*>(edge.second)->getLine(),
							make_pair(false, true)
					)	);
			}	}

			// Arrivals
			if(withArrivals)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, getArrivalEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPatternsMap::iterator it(journeyPatterns.find(static_cast<const LineStop*>(edge.second)->getLine()));
					if(it == journeyPatterns.end())
					{
						journeyPatterns.insert(
							make_pair(
								static_cast<const LineStop*>(edge.second)->getLine(),
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
			bool withStopAreaData /*= true*/,
			const CoordinatesSystem& coordinatesSystem /*= CoordinatesSystem::GetInstanceCoordinatesSystem()*/,
			std::string prefix /*= std::string() */
		) const	{

			// Main data
			pm.insert(DATA_ID, getKey());
			pm.insert(DATA_NAME, getName());
			pm.insert(DATA_OPERATOR_CODE, getCodeBySources());
			if(getGeometry().get())
			{
				boost::shared_ptr<Point> gp = coordinatesSystem.convertPoint(*getGeometry());
				if(gp.get())
				{
					pm.insert(DATA_X, gp->getX());
					pm.insert(DATA_Y, gp->getY());
				}
			}

			// Stop area data
			if(withStopAreaData)
			{
				boost::shared_ptr<ParametersMap> stopAreaPM(new ParametersMap);
				getConnectionPlace()->toParametersMap(*stopAreaPM, &coordinatesSystem);
				pm.insert(TAG_STOP_AREA, stopAreaPM);
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
}	}
