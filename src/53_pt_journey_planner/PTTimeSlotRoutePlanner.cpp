
/** PTTimeSlotRoutePlanner class implementation.
	@file PTTimeSlotRoutePlanner.cpp

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

#include "PTTimeSlotRoutePlanner.h"

#include "AlgorithmLogger.hpp"
#include "FreeDRTArea.hpp"
#include "Hub.h"
#include "IntegralSearcher.h"
#include "JourneysResult.h"
#include "Log.h"
#include "NamedPlace.h"
#include "Place.h"
#include "PTModule.h"
#include "RoadModule.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "VAMConverter.hpp"
#include "VertexAccessMap.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace algorithm;
	using namespace road;
	using namespace graph;
	using namespace geography;
	using namespace algorithm;
	using namespace util;
	using namespace pt;

	namespace pt_journey_planner
	{
		PTTimeSlotRoutePlanner::PTTimeSlotRoutePlanner(
			const geography::Place* origin,
			const geography::Place* destination,
			const ptime& lowerDepartureTime,
			const ptime& higherDepartureTime,
			const ptime& lowerArrivalTime,
			const ptime& higherArrivalTime,
			const boost::optional<std::size_t> maxSolutionsNumber,
			const graph::AccessParameters accessParameters,
			const PlanningOrder planningOrder,
			bool ignoreReservation,
			const AlgorithmLogger& logger,
			boost::optional<boost::posix_time::time_duration> maxTransferDuration,
			boost::optional<double> minMaxDurationRatioFilter,
			bool enableTheoretical,
			bool enableRealTime
		):	TimeSlotRoutePlanner(
				origin->getVertexAccessMap(
					accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				destination->getVertexAccessMap(
					accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				lowerDepartureTime, higherDepartureTime,
				lowerArrivalTime, higherArrivalTime,
				PTModule::GRAPH_ID,
				PTModule::GRAPH_ID,
				optional<posix_time::time_duration>(),
				maxSolutionsNumber,
				accessParameters,
				planningOrder,
				100,
				ignoreReservation,
				logger,
				maxTransferDuration,
				minMaxDurationRatioFilter,
				enableTheoretical,
				enableRealTime
			),
			_departurePlace(origin),
			_arrivalPlace(destination)
		{
		}



		PTRoutePlannerResult PTTimeSlotRoutePlanner::run() const
		{
			TimeSlotRoutePlanner::Result result;
			_logger.openTimeSlotJourneyPlannerLog();

			// Check if departure and arrival VAMs has contains at least one vertex
			if(_originVam.getMap().empty() ||
				_destinationVam.getMap().empty()
			){
				return PTRoutePlannerResult(_departurePlace, _arrivalPlace, false, result);
			}

			// Check if the departure and arrival places are the same
			if(_originVam.intersercts(_destinationVam))
			{
				return PTRoutePlannerResult(_departurePlace, _arrivalPlace, true, result);
			}

			// Search stops around the departure and arrival places using the road network
			// FIXME: Need to handle approcahSpeed = 0 in IntegralSearcher himself
			VertexAccessMap ovam, dvam;
			if(_accessParameters.getApproachSpeed() != 0)
			{
				VAMConverter extenderToPhysicalStops(
					_accessParameters,
					_logger,
					PTModule::GRAPH_ID,
					RoadModule::GRAPH_ID,
					getLowestDepartureTime(),
					getHighestDepartureTime(),
					getLowestArrivalTime(),
					getHighestArrivalTime()
				);
				ovam = extenderToPhysicalStops.run(
					_originVam,
					_destinationVam,
					DEPARTURE_TO_ARRIVAL
				);
				dvam = extenderToPhysicalStops.run(
					_destinationVam,
					_originVam,
					ARRIVAL_TO_DEPARTURE
				);
			}
			else
			{
				// FIXME: Need to exclude Roads part of VAM
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, _originVam.getMap())
				{
					const Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PTModule::GRAPH_ID)
					{
						ovam.insert(vertex, itps.second);
					}
				}
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, _destinationVam.getMap())
				{
					const Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PTModule::GRAPH_ID)
					{
						dvam.insert(vertex, itps.second);
					}
				}
			}

			// Log the vams
			_logger.logTimeSlotJourneyPlannerApproachMap(true, ovam);
			_logger.logTimeSlotJourneyPlannerApproachMap(false, dvam);

			// Handle of the case of possible full road approach
			if(	ovam.intersercts(dvam)
			){
				Journey resultJourney(ovam.getBestIntersection(dvam));
				ptime departureTime(resultJourney.getFirstDepartureTime());
				if(departureTime.time_of_day().seconds())
				{
					resultJourney.shift(seconds(60 - departureTime.time_of_day().seconds()));
				}
				resultJourney.shift(
					getLowestDepartureTime() - resultJourney.getFirstDepartureTime()
				);
				resultJourney.forceContinuousServiceRange(getHighestArrivalTime() - getLowestDepartureTime());

				result.push_back(resultJourney);
			}

			// Free DRT approach
			{
				_extendByFreeDRT(ovam, dvam, DEPARTURE_TO_ARRIVAL);
				_extendByFreeDRT(dvam, ovam, ARRIVAL_TO_DEPARTURE);
			}


			if(result.empty())
			{
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
					70, // 252 km/h TODO take it configurable
					_ignoreReservation,
					_logger,
					_maxTransferDuration,
					_minMaxDurationRatioFilter,
					_enableTheoretical,
					_enableRealTime
				);
				return PTRoutePlannerResult(
					_departurePlace,
					_arrivalPlace,
					false,
					r.run()
				);
			}
			else
			{
				TimeSlotRoutePlanner r(
					ovam,
					dvam,
					result.front(),
					_whatToSearch,
					_graphToUse,
					_maxDuration,
					_maxSolutionsNumber,
					_accessParameters,
					_planningOrder,
					100,
					_ignoreReservation,
					_logger,
					_maxTransferDuration,
					_minMaxDurationRatioFilter,
					_enableTheoretical,
					_enableRealTime
				);
				return PTRoutePlannerResult(
					_departurePlace,
					_arrivalPlace,
					false,
					r.run()
				);
			}
		}



		void PTTimeSlotRoutePlanner::_extendByFreeDRT(
			VertexAccessMap& vam,
			const VertexAccessMap& destinationVam,
			PlanningPhase direction
		) const	{
#if 0
			// TODO Work in progress...
			
			// Loop on each stop
			BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, vam.getMap())
			{
				// Select stop areas
				const StopPoint* stopPoint(dynamic_cast<const StopPoint*>(itps.first));
				if(!stopPoint)
				{
					continue;
				}

				// Free DRTs loop
				StopArea::FreeDRTs freeDRTs(stopPoint->getConnectionPlace()->getFreeDRTs());
				BOOST_FOREACH(const StopArea::FreeDRTs::value_type& freeDRT, freeDRTs)
				{
					// Select

				}
			}
#endif
		}
}	}
