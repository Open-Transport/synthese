
/** PublicBikeJourneyPlanner class implementation.
	@file PublicBikeJourneyPlanner.cpp

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

#include "PublicBikeJourneyPlanner.hpp"

#include "AlgorithmLogger.hpp"
#include "Hub.h"
#include "IntegralSearcher.h"
#include "NamedPlace.h"
#include "Log.h"
#include "Place.h"
#include "PTModule.h"
#include "PublicBikingModule.h"
#include "PublicBikeNetwork.hpp"
#include "PublicBikeStation.hpp"
#include "RoadModule.h"
#include "RoutePlanner.h"
#include "JourneysResult.h"
#include "VAMConverter.hpp"
#include "Vertex.h"
#include "VertexAccessMap.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace public_biking
	{
		PublicBikeJourneyPlanner::PublicBikeJourneyPlanner(
			const geography::Place* origin,
			const geography::Place* destination,
			const ptime& departureTime,
			const ptime& higherArrivalTime,
			const graph::AccessParameters accessParameters,
			const graph::AccessParameters journeyParameters,
			const algorithm::AlgorithmLogger& logger
		):	_departurePlace(origin),
			_arrivalPlace(destination),
			_journeyParameters(journeyParameters),
			_departureTime(departureTime),
			_higherArrivalTime(higherArrivalTime),
			_accessParameters(accessParameters),
			_logger(logger)
		{
		}



		PublicBikeJourneyPlannerResult PublicBikeJourneyPlanner::run() const
		{
			algorithm::TimeSlotRoutePlanner::Result result;

			// Search bike stations around the departure and arrival places using the road network
			graph::VertexAccessMap ovam, dvam;
			if(_accessParameters.getApproachSpeed() != 0)
			{
				algorithm::VAMConverter extenderToBikeStations(
					_accessParameters,
					_logger,
					PublicBikingModule::GRAPH_ID,
					road::RoadModule::GRAPH_ID,
					_departureTime,
					_departureTime,
					_departureTime,
					_higherArrivalTime,
					_departurePlace,
					_arrivalPlace
				);
				ovam = extenderToBikeStations.run(
					_departurePlace->getVertexAccessMap(_accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0),
					_arrivalPlace->getVertexAccessMap(_accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0),
					algorithm::DEPARTURE_TO_ARRIVAL
				);
				dvam = extenderToBikeStations.run(
					_arrivalPlace->getVertexAccessMap(_accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0),
					_departurePlace->getVertexAccessMap(_accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0),
					algorithm::ARRIVAL_TO_DEPARTURE
				);
			}
			else
			{
				// No approach possible, so we expect departure and arrival places are bike stations
				graph::VertexAccessMap originVam, destinationVam;
				originVam = _departurePlace->getVertexAccessMap(_accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0);
				destinationVam = _arrivalPlace->getVertexAccessMap(_accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0);
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itps, originVam.getMap())
				{
					const graph::Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PublicBikingModule::GRAPH_ID)
					{
						ovam.insert(vertex, itps.second);
					}
				}
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itps,destinationVam.getMap())
				{
					const graph::Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PublicBikingModule::GRAPH_ID)
					{
						dvam.insert(vertex, itps.second);
					}
				}
			}

			BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itobs, ovam.getMap())
			{
				// For each bike station around the origin position
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itdbs, dvam.getMap())
				{
					// For each bike station around the destination position
					// Road journey planner between origin bike station to destination bike station
					const graph::Vertex* originVertex(itobs.first);
					graph::VertexAccessMap roadOVam;
					originVertex->getHub()->getVertexAccessMap(roadOVam, PublicBikingModule::GRAPH_ID, *originVertex, true);
					const graph::Vertex* destinationVertex(itdbs.first);
					graph::VertexAccessMap roadDVam;
					destinationVertex->getHub()->getVertexAccessMap(roadDVam, PublicBikingModule::GRAPH_ID, *destinationVertex, true);

					if (dynamic_cast<const PublicBikeStation*>(originVertex) &&
						dynamic_cast<const PublicBikeStation*>(destinationVertex))
					{
						if (dynamic_cast<const PublicBikeStation*>(originVertex)->getPublicBikeNetwork() !=
							dynamic_cast<const PublicBikeStation*>(destinationVertex)->getPublicBikeNetwork())
						{
							continue;
						}
					}

					algorithm::RoutePlanner r(
						roadOVam,
						roadDVam,
						algorithm::DEPARTURE_FIRST,
						_journeyParameters,
						boost::optional<boost::posix_time::time_duration>(),
						_departureTime,
						_departureTime,
						_higherArrivalTime,
						road::RoadModule::GRAPH_ID,
						road::RoadModule::GRAPH_ID,
						_journeyParameters.getApproachSpeed(),
						true,
						_logger
					);

					graph::Journey journey(r.run());
					if(journey.empty())
					{
						util::Log::GetInstance().debug("PublicBikeJourneyPlanner::run no solution from " +
							lexical_cast<string>(originVertex->getKey()) + " to " +
							lexical_cast<string>(destinationVertex->getKey()));
						continue;
					}

					result.push_back(journey);
				}
			}

			return PublicBikeJourneyPlannerResult(_departurePlace, _arrivalPlace, true, result);
		}
}	}
