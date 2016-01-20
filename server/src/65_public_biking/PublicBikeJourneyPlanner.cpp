
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
#include "AStarShortestPathCalculator.hpp"
#include "Crossing.h"
#include "Edge.h"
#include "Hub.h"
#include "IntegralSearcher.h"
#include "NamedPlace.h"
#include "Log.h"
#include "Place.h"
#include "PTModule.h"
#include "PublicBikingModule.h"
#include "PublicBikeNetwork.hpp"
#include "PublicBikeStation.hpp"
#include "RoadChunkEdge.hpp"
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

			typedef std::map<PublicBikeNetwork*, graph::VertexAccessMap> NetworksMap;
			NetworksMap arrivalMap;

			BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itdbs, dvam.getMap())
			{
				// For each bike station around the destination position
				const graph::Vertex* destinationVertex(itdbs.first);
				if (dynamic_cast<const PublicBikeStation*>(destinationVertex))
				{
					if (arrivalMap.find(dynamic_cast<const PublicBikeStation*>(destinationVertex)->getPublicBikeNetwork()) == arrivalMap.end())
					{
						graph::VertexAccessMap newDvam;
						arrivalMap.insert(
							make_pair(
								dynamic_cast<const PublicBikeStation*>(destinationVertex)->getPublicBikeNetwork(),
								newDvam
						)	);
					}

					NetworksMap::iterator itNetwork = arrivalMap.find(dynamic_cast<const PublicBikeStation*>(destinationVertex)->getPublicBikeNetwork());
					graph::VertexAccessMap& networkDvam = (*itNetwork).second;
					networkDvam.insert(itdbs.first, itdbs.second);
				}
			}

			BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itobs, ovam.getMap())
			{
				// For each bike station around the origin position
				// Road journey planner between origin bike station to destination bike stationS
				const graph::Vertex* originVertex(itobs.first);
				graph::VertexAccessMap roadOvam;
				originVertex->getHub()->getVertexAccessMap(roadOvam, road::RoadModule::GRAPH_ID, *originVertex, true);
				if (!dynamic_cast<const PublicBikeStation*>(originVertex))
				{
					continue;
				}

				NetworksMap::iterator itNetworkArrival = arrivalMap.find(dynamic_cast<const PublicBikeStation*>(originVertex)->getPublicBikeNetwork());
				if (itNetworkArrival == arrivalMap.end())
				{
					// Do not search for network with no station at the arrival
					continue;
				}
				graph::VertexAccessMap bsDvam, roadDvam;
				bsDvam = (*itNetworkArrival).second;
				if (bsDvam.getMap().empty())
				{
					// Do not search for network with no station at the arrival
					continue;
				}
				// Construct arrival road VAMs containing approach journey to bike stations
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itdbs, bsDvam.getMap())
				{
					graph::VertexAccessMap roadItDVam;
					const graph::Vertex* destinationVertex(itdbs.first);
					destinationVertex->getHub()->getVertexAccessMap(roadItDVam, road::RoadModule::GRAPH_ID, *destinationVertex, true);
					// Complete with approach to bike station
					BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itc, roadItDVam.getMap())
					{
						graph::Journey journeyToBikeStation = itdbs.second.approachJourney;
						journeyToBikeStation.prepend(
							_constructEndJourney(
								itc.first,
								dynamic_cast<const PublicBikeStation*>(destinationVertex),
								itc.second.approachTime + itdbs.second.approachTime,
								itc.second
							)
						);
						roadDvam.insert(
							itc.first,
							graph::VertexAccess(
								itc.second.approachTime + itdbs.second.approachTime,
								itc.second.approachDistance + itdbs.second.approachDistance,
								journeyToBikeStation
						)	);
					}
				}

				algorithm::RoutePlanner r(
					roadOvam,
					roadDvam,
					algorithm::DEPARTURE_FIRST,
					_journeyParameters,
					boost::optional<boost::posix_time::time_duration>(),
					_departureTime + itobs.second.approachTime,
					_departureTime + itobs.second.approachTime,
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
					util::Log::GetInstance().debug("PublicBikeJourneyPlanner::run no solution for network " +
						lexical_cast<string>((*itNetworkArrival).first->getKey()));
					continue;
				}

				// Approach legs at the origin
				journey.prepend(
					_constructStartJourney(
						dynamic_cast<const PublicBikeStation*>(originVertex),
						journey.getFirstJourneyLeg(),
						roadOvam.getVertexAccess(journey.getFirstJourneyLeg().getRealTimeDepartureVertex())
				)	);
				journey.prepend(itobs.second.approachJourney);

				result.push_back(journey);
			}

			return PublicBikeJourneyPlannerResult(_departurePlace, _arrivalPlace, true, result);
		}

		std::vector<PublicBikeJourneyPlanner::PublicBikeJourneyPlannerAStarResult> PublicBikeJourneyPlanner::runAStar() const
		{
			std::vector<PublicBikeJourneyPlannerAStarResult> result;

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

			typedef std::map<PublicBikeNetwork*, graph::VertexAccessMap> NetworksMap;
			NetworksMap arrivalMap;

			BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itdbs, dvam.getMap())
			{
				// For each bike station around the destination position
				const graph::Vertex* destinationVertex(itdbs.first);
				if (dynamic_cast<const PublicBikeStation*>(destinationVertex))
				{
					if (arrivalMap.find(dynamic_cast<const PublicBikeStation*>(destinationVertex)->getPublicBikeNetwork()) == arrivalMap.end())
					{
						graph::VertexAccessMap newDvam;
						arrivalMap.insert(
							make_pair(
								dynamic_cast<const PublicBikeStation*>(destinationVertex)->getPublicBikeNetwork(),
								newDvam
						)	);
					}

					NetworksMap::iterator itNetwork = arrivalMap.find(dynamic_cast<const PublicBikeStation*>(destinationVertex)->getPublicBikeNetwork());
					graph::VertexAccessMap& networkDvam = (*itNetwork).second;
					networkDvam.insert(itdbs.first, itdbs.second);
				}
			}

			BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itobs, ovam.getMap())
			{
				// For each bike station around the origin position
				// Road journey planner between origin bike station to destination bike stationS
				const graph::Vertex* originVertex(itobs.first);
				graph::VertexAccessMap roadOvam;
				originVertex->getHub()->getVertexAccessMap(roadOvam, road::RoadModule::GRAPH_ID, *originVertex, true);
				if (!dynamic_cast<const PublicBikeStation*>(originVertex))
				{
					continue;
				}

				NetworksMap::iterator itNetworkArrival = arrivalMap.find(dynamic_cast<const PublicBikeStation*>(originVertex)->getPublicBikeNetwork());
				graph::VertexAccessMap bsDvam = (*itNetworkArrival).second;
				if (bsDvam.getMap().empty())
				{
					// Do not search for network with no station at the arrival
					continue;
				}

				graph::VertexAccessMap roadDvam;
				// Construct arrival road VAMs containing approach journey to bike stations
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itdbs, bsDvam.getMap())
				{
					graph::VertexAccessMap roadItDVam;
					const graph::Vertex* destinationVertex(itdbs.first);
					destinationVertex->getHub()->getVertexAccessMap(roadItDVam, road::RoadModule::GRAPH_ID, *destinationVertex, true);
					// Complete with approach to bike station
					BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itc, roadItDVam.getMap())
					{
						graph::Journey journeyToBikeStation = itdbs.second.approachJourney;
						journeyToBikeStation.prepend(
							_constructEndJourney(
								itc.first,
								dynamic_cast<const PublicBikeStation*>(destinationVertex),
								itc.second.approachTime + itdbs.second.approachTime,
								itc.second
						)	);
						roadDvam.insert(
							itc.first,
							graph::VertexAccess(
								itc.second.approachTime + itdbs.second.approachTime,
								itc.second.approachDistance + itdbs.second.approachDistance,
								journeyToBikeStation
						)	);
					}
				}

				// Astar algorithm
				algorithm::AStarShortestPathCalculator r(
					roadOvam,
					roadDvam,
					_departureTime,
					_journeyParameters
				);

				algorithm::AStarShortestPathCalculator::ResultPath path(r.run());
				if(!path.empty())
				{
					// Reconstruct approach journey
					PublicBikeJourneyPlannerAStarResult subResult;
					if (roadDvam.contains((*(path.rbegin()))->getFromVertex()))
					{
						graph::Journey startJourney(itobs.second.approachJourney);
						startJourney.append(
							_constructAStarStartJourney(
								dynamic_cast<const PublicBikeStation*>(originVertex),
								*(path.begin()),
								itobs.second.approachTime,
								roadOvam.contains((*(path.begin()))->getFromVertex()) ?
									roadOvam.getVertexAccess((*(path.begin()))->getFromVertex()) :
									roadOvam.getVertexAccess((*(path.begin()))->getNext()->getFromVertex())
						)	);
						subResult = boost::make_tuple(
							startJourney,
							path,
							roadDvam.getVertexAccess((*(path.rbegin()))->getFromVertex()).approachJourney
						);
					}
					else if (roadDvam.contains((*(path.rbegin()))->getNext()->getFromVertex()))
					{
						graph::Journey startJourney(itobs.second.approachJourney);
						startJourney.append(
							_constructAStarStartJourney(
								dynamic_cast<const PublicBikeStation*>(originVertex),
								*(path.begin()),
								itobs.second.approachTime,
								roadOvam.contains((*(path.begin()))->getFromVertex()) ?
									roadOvam.getVertexAccess((*(path.begin()))->getFromVertex()) :
									roadOvam.getVertexAccess((*(path.begin()))->getNext()->getFromVertex())
						)	);
						subResult = boost::make_tuple(
							startJourney,
							path,
							roadDvam.getVertexAccess((*(path.rbegin()))->getNext()->getFromVertex()).approachJourney
						);
					}

					result.push_back(subResult);
				}
			}

			return result;
		}

		graph::ServicePointer PublicBikeJourneyPlanner::_constructStartJourney(
			const PublicBikeStation* origin,
			const graph::ServicePointer firstSP,
			graph::VertexAccess va
		) const
		{
			graph::ServicePointer startSP;
			startSP.setDepartureInformations(
				origin->getProjectedPoint().getRoadChunk()->getForwardEdge(),
				firstSP.getDepartureDateTime() - va.approachTime,
				firstSP.getDepartureDateTime() - va.approachTime,
				*origin
			);
			startSP.setArrivalInformations(
				*(firstSP.getDepartureEdge()),
				firstSP.getDepartureDateTime(),
				firstSP.getDepartureDateTime(),
				*(firstSP.getDepartureEdge()->getFromVertex())
			);
			startSP.setUserClassRank(_journeyParameters.getUserClassRank());

			return startSP;
		}

		graph::ServicePointer PublicBikeJourneyPlanner::_constructEndJourney(
			const graph::Vertex* lastVertex,
			const PublicBikeStation* destination,
			const boost::posix_time::time_duration& timeShiftCrossing,
			graph::VertexAccess va
		) const
		{
			graph::ServicePointer endSP;
			endSP.setDepartureInformations(
				*((*(lastVertex->getDepartureEdges().begin())).second),
				_departureTime - timeShiftCrossing,
				_departureTime - timeShiftCrossing,
				*(lastVertex)
			);
			endSP.setArrivalInformations(
				destination->getProjectedPoint().getRoadChunk()->getForwardEdge(),
				_departureTime - timeShiftCrossing + va.approachTime,
				_departureTime - timeShiftCrossing + va.approachTime,
				*destination
			);
			endSP.setUserClassRank(_journeyParameters.getUserClassRank());

			return endSP;
		}

		graph::ServicePointer PublicBikeJourneyPlanner::_constructAStarStartJourney(
			const PublicBikeStation* origin,
			const road::RoadChunkEdge* firstRoadChunkEdge,
			const boost::posix_time::time_duration& timeToReachBikeStation,
			const graph::VertexAccess va
		) const
		{
			graph::ServicePointer startSP;
			startSP.setDepartureInformations(
				origin->getProjectedPoint().getRoadChunk()->getForwardEdge(),
				_departureTime + timeToReachBikeStation,
				_departureTime + timeToReachBikeStation,
				*origin
			);
			startSP.setArrivalInformations(
				*firstRoadChunkEdge,
				_departureTime + timeToReachBikeStation + va.approachTime,
				_departureTime + timeToReachBikeStation + va.approachTime,
				*(firstRoadChunkEdge->getFromVertex())
			);
			startSP.setUserClassRank(_journeyParameters.getUserClassRank());

			return startSP;
		}
}	}
