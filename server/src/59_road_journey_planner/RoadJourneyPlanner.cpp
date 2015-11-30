
/** RoadJourneyPlanner class implementation.
	@file RoadJourneyPlanner.cpp

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

#include "RoadJourneyPlanner.h"

#include "AlgorithmLogger.hpp"
#include "Hub.h"
#include "PTModule.h"
#include "RoadModule.h"
#include "Place.h"
#include "Log.h"
#include "JourneysResult.h"
#include "VertexAccessMap.h"
#include "IntegralSearcher.h"
#include "StopPoint.hpp"
#include "NamedPlace.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace road;
	using namespace graph;
	using namespace geography;
	using namespace algorithm;
	using namespace util;

	namespace road_journey_planner
	{
		RoadJourneyPlanner::RoadJourneyPlanner(
			const geography::Place* origin,
			const geography::Place* destination,
			const ptime& lowerDepartureTime,
			const ptime& higherDepartureTime,
			const ptime& lowerArrivalTime,
			const ptime& higherArrivalTime,
			const boost::optional<std::size_t> maxSolutionsNumber,
			const graph::AccessParameters accessParameters,
			const PlanningOrder planningOrder,
			const AlgorithmLogger& logger,
			const bool allowSmallPlanning
		):	TimeSlotRoutePlanner(
				origin->getVertexAccessMap(
					accessParameters, RoadModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				destination->getVertexAccessMap(
					accessParameters, RoadModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				lowerDepartureTime, higherDepartureTime,
				lowerArrivalTime, higherArrivalTime,
				RoadModule::GRAPH_ID,
				RoadModule::GRAPH_ID,
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
			_allowSmallPlanning(allowSmallPlanning)
		{
		}



		RoadJourneyPlannerResult RoadJourneyPlanner::run() const
		{
			TimeSlotRoutePlanner::Result result;

			// Control if departure and arrival VAMs has contains at least one vertex
			// or if the departure and arrival places are the same
			if(	_originVam.getMap().empty() ||
				_destinationVam.getMap().empty() ||
				(!_allowSmallPlanning && _originVam.intersercts(_destinationVam))
			){
				return RoadJourneyPlannerResult(_departurePlace, _arrivalPlace, true, result);
			}

			VertexAccessMap ovam(_originVam);
			VertexAccessMap dvam(_destinationVam);

			// Log vams
			_logger.logTimeSlotJourneyPlannerApproachMap(true, ovam);
			_logger.logTimeSlotJourneyPlannerApproachMap(false, dvam);

// 			if (result.empty())
// 			{
// 				_previousContinuousServiceDuration = posix_time::minutes(0);
// 			}
// 			else
// 			{
// 				shared_ptr<Journey> journey(_result.front());
// 				_previousContinuousServiceDuration = journey->getDuration();
// 				_previousContinuousServiceLastDeparture = journey->getDepartureTime();
// 				_previousContinuousServiceLastDeparture += journey->getContinuousServiceRange();
// 			}
// TODO prepare continuous service

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
				_accessParameters,
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
			return RoadJourneyPlannerResult(
				_departurePlace,
				_arrivalPlace,
				false,
				r.run()
			);
		}
}	}
