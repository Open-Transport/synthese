
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
#include "PTModule.h"
#include "RoadModule.h"
#include "Place.h"
#include "PublicBikingModule.h"
#include "PublicBikeNetwork.hpp"
#include "PublicBikeStation.hpp"
#include "Log.h"
#include "JourneysResult.h"
#include "VertexAccessMap.h"
#include "IntegralSearcher.h"
#include "NamedPlace.h"
#include "VAMConverter.hpp"
#include "Vertex.h"

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
			const ptime& lowerDepartureTime,
			const ptime& higherDepartureTime,
			const ptime& lowerArrivalTime,
			const ptime& higherArrivalTime,
			const boost::optional<std::size_t> maxSolutionsNumber,
			const graph::AccessParameters accessParameters,
			const graph::AccessParameters journeyParameters,
			const algorithm::PlanningOrder planningOrder,
			const algorithm::AlgorithmLogger& logger
		):	TimeSlotRoutePlanner(
				origin->getVertexAccessMap(
					accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0
				),
				destination->getVertexAccessMap(
					accessParameters, PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0
				),
				lowerDepartureTime, higherDepartureTime,
				lowerArrivalTime, higherArrivalTime,
				PublicBikingModule::GRAPH_ID,
				road::RoadModule::GRAPH_ID,
				optional<posix_time::time_duration>(),
				maxSolutionsNumber,
				accessParameters,
				planningOrder,
				accessParameters.getApproachSpeed(),
				true,
				logger,
				boost::optional<boost::posix_time::time_duration>(),
				boost::optional<double>(),
				true,
				true,
				graph::UseRule::RESERVATION_INTERNAL_DELAY,
				true
			),
			_departurePlace(origin),
			_arrivalPlace(destination),
			_journeyParameters(journeyParameters)
		{
		}



		PublicBikeJourneyPlannerResult PublicBikeJourneyPlanner::run() const
		{
			TimeSlotRoutePlanner::Result result;

			// Check if departure and arrival VAMs has contains at least one vertex
			if(_originVam.getMap().empty() ||
				_destinationVam.getMap().empty()
			){
				_logger.closeTimeSlotJourneyPlannerLog();
				return PublicBikeJourneyPlannerResult(_departurePlace, _arrivalPlace, false, result);
			}

			// Check if the departure and arrival places are the same
			if(_originVam.intersercts(_destinationVam))
			{
				_logger.closeTimeSlotJourneyPlannerLog();
				return PublicBikeJourneyPlannerResult(_departurePlace, _arrivalPlace, true, result);
			}

			// Search bike stations around the departure and arrival places using the road network
			// FIXME: Need to handle approcahSpeed = 0 in IntegralSearcher himself
			graph::VertexAccessMap ovam, dvam;
			if(_accessParameters.getApproachSpeed() != 0)
			{
				algorithm::VAMConverter extenderToBikeStations(
					_accessParameters,
					_logger,
					PublicBikingModule::GRAPH_ID,
					road::RoadModule::GRAPH_ID,
					getLowestDepartureTime(),
					getHighestDepartureTime(),
					getLowestArrivalTime(),
					getHighestArrivalTime(),
					_departurePlace,
					_arrivalPlace
				);
				ovam = extenderToBikeStations.run(
					_originVam,
					_destinationVam,
					algorithm::DEPARTURE_TO_ARRIVAL
				);
				dvam = extenderToBikeStations.run(
					_destinationVam,
					_originVam,
					algorithm::ARRIVAL_TO_DEPARTURE
				);
			}
			else
			{
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itps, _originVam.getMap())
				{
					const graph::Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PublicBikingModule::GRAPH_ID)
					{
						ovam.insert(vertex, itps.second);
					}
				}
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& itps, _destinationVam.getMap())
				{
					const graph::Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PublicBikingModule::GRAPH_ID)
					{
						dvam.insert(vertex, itps.second);
					}
				}
			}

			// Log the vams
			_logger.logTimeSlotJourneyPlannerApproachMap(true, ovam);
			_logger.logTimeSlotJourneyPlannerApproachMap(false, dvam);

			TimeSlotRoutePlanner r(
				ovam,
				dvam,
				getLowestDepartureTime(),
				getHighestDepartureTime(),
				getLowestArrivalTime(),
				getHighestArrivalTime(),
				_whatToSearch,
				_graphToUse,
				_maxDuration,
				_maxSolutionsNumber,
				_journeyParameters,
				_planningOrder,
				_vmax,
				true,
				_logger,
				boost::optional<boost::posix_time::time_duration>(),
				boost::optional<double>(),
				true,
				true,
				graph::UseRule::RESERVATION_INTERNAL_DELAY,
				true
			);
			return PublicBikeJourneyPlannerResult(
				_departurePlace,
				_arrivalPlace,
				false,
				r.run()
			);
		}
}	}
