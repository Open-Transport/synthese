
/** RoutePlanner class implementation.
	@file RoutePlanner.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "RoutePlanner.h"

#include "IntegralSearcher.h"
#include "BestVertexReachesMap.h"
#include "Hub.h"
#include "Edge.h"
#include "Service.h"
#include "Vertex.h"
#include "Journey.h"
#include "VertexAccessMap.h"
#include "RoutePlannerLogger.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <limits>

#undef max
#undef min

using namespace boost;
using namespace std;
using namespace boost::posix_time;


namespace synthese
{
	using namespace geography;
	using namespace util;
	using namespace graph;


	namespace algorithm
	{
		RoutePlanner::RoutePlanner(
			const graph::VertexAccessMap& originVam,
			const graph::VertexAccessMap& destinationVam,
			PlanningOrder planningOrder, /*!< Define planning sequence. */
			graph::AccessParameters accessParameters,
			optional<posix_time::time_duration> maxDuration,
			const ptime& minBeginTime,
			const ptime& maxBeginTime,
			const ptime& maxEndTime,
			graph::GraphIdType whatToSearch,
			graph::GraphIdType graphToUse,
			double vmax,
			bool ignoreReservation,
			std::ostream* logStream /*= NULL*/,
			boost::optional<const JourneyTemplates&> journeyTemplates,
			const optional<time_duration>	maxTransferDuration
		):	_originVam(originVam),
			_destinationVam(destinationVam),
			_planningOrder(planningOrder),
			_accessParameters(accessParameters),
			_maxDuration(maxDuration),
			_minBeginTime(minBeginTime),
			_maxBeginTime(maxBeginTime),
			_maxEndTime(maxEndTime),
			_whatToSearch(whatToSearch),
			_graphToUse(graphToUse),
			_logStream(logStream),
			_totalDistance(
				(destinationVam.getCentroid().get() && originVam.getCentroid().get()) ?
				destinationVam.getCentroid()->distance(originVam.getCentroid().get()) :
				numeric_limits<int>::max()
			),
			_journeyTemplates(journeyTemplates),
			_vmax(vmax),
			_ignoreReservation(ignoreReservation),
			_maxTransferDuration(maxTransferDuration)
		{
		}



		Journey RoutePlanner::run(
		){
//			if(_originVam.intersercts(_destinationVam)) throw SamePlacesException();

			Result result(_planningOrder == DEPARTURE_FIRST ? DEPARTURE_TO_ARRIVAL : ARRIVAL_TO_DEPARTURE);

			// Look for best time
			_findBestJourney(
				result,
				_planningOrder == DEPARTURE_FIRST ? _originVam : _destinationVam,
				_planningOrder == DEPARTURE_FIRST ? _destinationVam : _originVam,
				_planningOrder == DEPARTURE_FIRST ? DEPARTURE_TO_ARRIVAL : ARRIVAL_TO_DEPARTURE,
				_minBeginTime,
				_maxBeginTime,
				_maxEndTime,
				false,
				_maxDuration
			);

			if(result.empty()) return result;

			Result result2(result, _planningOrder == DEPARTURE_FIRST ? ARRIVAL_TO_DEPARTURE : DEPARTURE_TO_ARRIVAL);

			ptime beginBound(result2.getBeginTime());
			ptime endBound(result2.getEndTime());
			if(	_maxDuration &&
				result2.getDuration() > _maxDuration
			){
				endBound =
					_planningOrder == DEPARTURE_FIRST ?
					result2.getFirstArrivalTime() - *_maxDuration :
					result2.getFirstDepartureTime() + *_maxDuration
				;

				if(	_planningOrder == DEPARTURE_FIRST && result2.getEndTime() < endBound ||
					result2.getEndTime() > endBound
				){
					result2.clear();
				}
			}

			// Look for best duration
			_findBestJourney(
				result2,
				_planningOrder == DEPARTURE_FIRST ? _destinationVam : _originVam,
				_planningOrder == DEPARTURE_FIRST ? _originVam : _destinationVam,
				_planningOrder == DEPARTURE_FIRST ? ARRIVAL_TO_DEPARTURE : DEPARTURE_TO_ARRIVAL,
				beginBound,
				beginBound,
				endBound,
				true,
				_maxDuration
			);

			if(result2.empty()) return Journey();

			// Inclusion of approach journeys in the result
			Journey finalResult;

			if (result2.getStartApproachDuration().total_seconds())
			{
				Journey originApproachJourney(
					_originVam.getVertexAccess(result2.getOrigin()->getFromVertex()).approachJourney
				);
				if (!originApproachJourney.empty())
				{
					originApproachJourney.shift(
						result2.getFirstDepartureTime() - originApproachJourney.getFirstDepartureTime()
					);
					originApproachJourney.forceContinuousServiceRange(result2.getContinuousServiceRange());
					finalResult.append(originApproachJourney);
				}
			}

			finalResult.append(static_cast<Journey>(result2));

			if (result2.getEndApproachDuration().total_seconds())
			{
				Journey goalApproachJourney(
					_destinationVam.getVertexAccess(result2.getDestination()->getFromVertex()).approachJourney
				);
				if (!goalApproachJourney.empty())
				{
					goalApproachJourney.shift(
						result2.getFirstArrivalTime() - goalApproachJourney.getFirstArrivalTime()
					);
					goalApproachJourney.forceContinuousServiceRange(result2.getContinuousServiceRange());
					finalResult.append(goalApproachJourney);
				}
			}

			return finalResult;
		}

// -------------------------------------------------------------------------- Recursion

		void RoutePlanner::_findBestJourney(
			RoutePlanningIntermediateJourney& result,
			const graph::VertexAccessMap& startVam,
			const graph::VertexAccessMap& endVam,
			PlanningPhase accessDirection,
			const ptime& originDateTime,
			const ptime& minMaxDateTimeAtOrigin,
			const ptime& minMaxDateTimeAtDestination,
			bool secondTime,
			boost::optional<boost::posix_time::time_duration> maxDuration
		){
			if(accessDirection == DEPARTURE_TO_ARRIVAL &&
				(originDateTime > minMaxDateTimeAtOrigin || originDateTime > minMaxDateTimeAtDestination || minMaxDateTimeAtOrigin > minMaxDateTimeAtDestination)
			){
				assert(false);
				return;
			}
			if(accessDirection == ARRIVAL_TO_DEPARTURE &&
				(originDateTime < minMaxDateTimeAtOrigin || originDateTime < minMaxDateTimeAtDestination || minMaxDateTimeAtOrigin < minMaxDateTimeAtDestination)
			){
				assert(false);
				return;
			}

			ptime __minMaxDateTimeAtDestination(minMaxDateTimeAtDestination);

			JourneysResult todo(originDateTime, accessDirection);

			ptime bestEndTime(minMaxDateTimeAtDestination);
			ptime lastBestEndTime(minMaxDateTimeAtDestination);

			// Initialization of the best vertex reaches map
			BestVertexReachesMap bestVertexReachesMap(accessDirection, startVam, endVam);

			optional<RoutePlannerLogger> logger(
				_logStream ?
				optional<RoutePlannerLogger>(RoutePlannerLogger(*_logStream, todo, result)) :
				optional<RoutePlannerLogger>()
			);
			if(logger)
			{
				logger->open();
			}

			// Initialization of the integral searcher
			IntegralSearcher is(
				accessDirection,
				_accessParameters,
				_whatToSearch,
				true,
				_graphToUse,
				todo,
				bestVertexReachesMap,
				endVam,
				originDateTime,
				minMaxDateTimeAtOrigin,
				__minMaxDateTimeAtDestination,
				secondTime,
				secondTime,
				_maxDuration,
				_vmax,
				_ignoreReservation,
				NULL,
				_totalDistance,
				_journeyTemplates
			);

			is.integralSearch(
				startVam,
				optional<size_t>(0),
				result.empty() ?
					optional<posix_time::time_duration>() :
					optional<posix_time::time_duration>(
						accessDirection == DEPARTURE_TO_ARRIVAL ?
						result.getFirstArrivalTime() - originDateTime :
						originDateTime - result.getFirstDepartureTime()
					)
			);

			// Main loop
			while(true)
			{
				if(logger)
				{
					logger->recordIntegralSearch(todo);
				}

				bool resultFound(false);

				// Take into account of the end reached journeys
				for(JourneysResult::ResultSet::const_iterator it(todo.getJourneys().begin());
					it != todo.getJourneys().end();
				){
					JourneysResult::ResultSet::const_iterator next(it);
					++next;
					const RoutePlanningIntermediateJourney& journey(*it->first);

					if (!journey.getEndReached())
						break;

					// A destination without any approach time stops the recursion
					const Vertex* reachedVertex(journey.getEndEdge().getFromVertex());
					const VertexAccess& va = endVam.getVertexAccess(reachedVertex);

					// Attempt to elect the solution as the result
					if (journey > result)
					{
						result = journey;
						if(logger)
						{
							logger->recordNewResult(result);
						}
						resultFound = true;
						bestEndTime = result.getEndTime();
					}

					if (va.approachTime.total_seconds() == 0)
					{
						todo.remove(it->first);
					}

					it = next;
				}

				if(resultFound)
				{
					// Removes useless branches according to the new result
					todo.cleanup(
						lastBestEndTime != bestEndTime,
						bestEndTime,
						bestVertexReachesMap,
						true,
						!secondTime,
						is
					);
				}

				// End of the algorithm
				if(todo.empty())
				{
					break;
				}

				if(logger)
				{
					logger->recordCleanup(todo);
				}

				// Recursion from the next reached point
				lastBestEndTime = bestEndTime;
				shared_ptr<const RoutePlanningIntermediateJourney> journey(todo.front());

				is.integralSearch(
					*journey,
					optional<size_t>(0),
					result.empty() ?
						optional<posix_time::time_duration>() :
						optional<posix_time::time_duration>(
							accessDirection == DEPARTURE_TO_ARRIVAL ?
							result.getFirstDepartureTime() - originDateTime :
							originDateTime - result.getFirstDepartureTime()
						),
					_maxTransferDuration
				);

			}

			if(logger)
			{
				logger->close();
			}
		}



		RoutePlanner::SamePlacesException::SamePlacesException()
			: Exception("Same places in route planner")
		{

		}
	}
}
