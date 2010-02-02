
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
#include "JourneyComparator.h"
#include "RoutePlannerLogger.h"

#include <algorithm>
#include <set>
#include <sstream>

#undef max
#undef min

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace geography;
	using namespace geometry;
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
			const time::DateTime& minBeginTime,
			const time::DateTime& maxBeginTime,
			const time::DateTime& maxEndTime,
			graph::GraphIdType whatToSearch,
			graph::GraphIdType graphToUse,
			std::ostream* logStream /*= NULL*/,
			boost::optional<const JourneyTemplates&> journeyTemplates
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
			_totalDistance(destinationVam.getIsobarycenter().getDistanceTo(originVam.getIsobarycenter())),
			_journeyTemplates(journeyTemplates)
		{
		}



		RoutePlanner::Result RoutePlanner::run(
		){
			if(_originVam.intersercts(_destinationVam)) throw SamePlacesException();

			Result result;

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
		    
			result.reverse();

			DateTime beginBound(result.getBeginTime());
			DateTime endBound(result.getEndTime());
			if(	_maxDuration &&
				result.getDuration() > _maxDuration
			){
				endBound = 
					_planningOrder == DEPARTURE_FIRST ?
					result.getArrivalTime() - _maxDuration->total_seconds() / 60 :
					result.getDepartureTime() + _maxDuration->total_seconds() / 60 
				;

				if(	_planningOrder == DEPARTURE_FIRST && result.getEndTime() < endBound ||
					result.getEndTime() > endBound
				){
					result.clear();
				}
			}

			// Look for best duration
			_findBestJourney(
				result,
				_planningOrder == DEPARTURE_FIRST ? _destinationVam : _originVam,
				_planningOrder == DEPARTURE_FIRST ? _originVam : _destinationVam,
				_planningOrder == DEPARTURE_FIRST ? ARRIVAL_TO_DEPARTURE : DEPARTURE_TO_ARRIVAL,
				beginBound,
				beginBound,
				endBound,
				true,
				_maxDuration
			);

			if(result.empty()) return result;

			if(_planningOrder == DEPARTURE_FIRST)
			{
				// Inclusion of approach journeys in the result
				if (result.getStartApproachDuration().total_seconds())
				{
					result.setStartApproachDuration(posix_time::minutes(0));
					Journey goalApproachJourney(
						_destinationVam.getVertexAccess(result.getDestination()->getFromVertex()).approachJourney
						);
					if (!goalApproachJourney.empty())
					{
						goalApproachJourney.shift(
							result.getArrivalTime().getSecondsDifference(goalApproachJourney.getDepartureTime()) +
							result.getDestination()->getHub()->getTransferDelay(
							*result.getDestination()->getFromVertex(),
							*goalApproachJourney.getOrigin()->getFromVertex()
							),
							result.getContinuousServiceRange()
							);
						goalApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
						result = Journey(goalApproachJourney, result);
					}
				}

				result.reverse();

				if (result.getStartApproachDuration().total_seconds())
				{
					result.setStartApproachDuration(posix_time::minutes(0));
					Journey originApproachJourney(
						_originVam.getVertexAccess(result.getOrigin()->getFromVertex()).approachJourney
						);
					if (!originApproachJourney.empty())
					{
						originApproachJourney.shift(
							result.getDepartureTime().getSecondsDifference(originApproachJourney.getDepartureTime()) -
							originApproachJourney.getDuration() -
							result.getOrigin()->getHub()->getTransferDelay(
							*originApproachJourney.getDestination()->getFromVertex(),
							*result.getOrigin()->getFromVertex()
							),
							result.getContinuousServiceRange()
							);
						originApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
						result = Journey(originApproachJourney, result);
					}
				}
			}
			else
			{
				if (result.getStartApproachDuration().total_seconds())
				{
					result.setStartApproachDuration(posix_time::minutes(0));
					Journey originApproachJourney(
						_originVam.getVertexAccess(result.getOrigin()->getFromVertex()).approachJourney
					);
					if (!originApproachJourney.empty())
					{
						originApproachJourney.shift(
							result.getDepartureTime().getSecondsDifference(originApproachJourney.getDepartureTime()) -
							originApproachJourney.getDuration() -
							result.getOrigin()->getHub()->getTransferDelay(
								*originApproachJourney.getDestination()->getFromVertex(),
								*result.getOrigin()->getFromVertex()
							),
							result.getContinuousServiceRange()
						);
						originApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
						result = Journey(originApproachJourney, result);
					}
				}

				result.reverse();

				// Inclusion of approach journeys in the result
				if (result.getStartApproachDuration().total_seconds())
				{
					result.setStartApproachDuration(posix_time::minutes(0));
					Journey goalApproachJourney(
						_destinationVam.getVertexAccess(result.getDestination()->getFromVertex()).approachJourney
					);
					if (!goalApproachJourney.empty())
					{
						goalApproachJourney.shift(
							result.getArrivalTime().getSecondsDifference(goalApproachJourney.getDepartureTime()) +
							result.getDestination()->getHub()->getTransferDelay(
							*result.getDestination()->getFromVertex(),
							*goalApproachJourney.getOrigin()->getFromVertex()
							),
							result.getContinuousServiceRange()
							);
						goalApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
						result = Journey(goalApproachJourney, result);
					}
				}
			}

			return result;
		}

// -------------------------------------------------------------------------- Recursion

		void RoutePlanner::_findBestJourney(
			Journey& result,
			const graph::VertexAccessMap& startVam,
			const graph::VertexAccessMap& endVam,
			AccessDirection accessDirection,
			const time::DateTime& originDateTime,
			const time::DateTime& minMaxDateTimeAtOrigin,
			const time::DateTime& minMaxDateTimeAtDestination,
			bool secondTime,
			boost::optional<boost::posix_time::time_duration> maxDuration
		){
			assert(accessDirection != UNDEFINED_DIRECTION);
			assert(result.empty() || result.getMethod() == accessDirection);
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

			DateTime __minMaxDateTimeAtDestination(minMaxDateTimeAtDestination);

			JourneysResult<JourneyComparator> todo(originDateTime);
			
			DateTime bestEndTime(minMaxDateTimeAtDestination);
			DateTime lastBestEndTime(minMaxDateTimeAtDestination);
			
			// Initialization of the best vertex reaches map
			BestVertexReachesMap bestVertexReachesMap(accessDirection, startVam);

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
						result.getEndTime().getSecondsDifference(originDateTime) :
						originDateTime.getSecondsDifference(result.getEndTime())
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
				for(JourneysResult<JourneyComparator>::ResultSet::const_iterator it(todo.getJourneys().begin());
					it != todo.getJourneys().end();
				){
					JourneysResult<JourneyComparator>::ResultSet::const_iterator next(it);
					++next;
					const Journey& journey(*it->first);
					
					if (!journey.getEndReached())
						break;
					
					// A destination without any approach time stops the recursion
					const Vertex* reachedVertex(journey.getEndEdge()->getFromVertex());
					const VertexAccess& va = endVam.getVertexAccess(reachedVertex);
					
					// Attempt to elect the solution as the result
					if (journey.isBestThan(result))
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
					todo.cleanup(
						lastBestEndTime != bestEndTime,
						bestEndTime,
						bestVertexReachesMap,
						true,
						!secondTime,
						is
					);
				}

				if(todo.empty())
				{
					break;
				}

				if(logger)
				{
					logger->recordCleanup(todo);
				}

				lastBestEndTime = bestEndTime;

				shared_ptr<const Journey> journey(todo.front());

				is.integralSearch(
					*journey,
					optional<size_t>(0),
					result.empty() ?
						optional<posix_time::time_duration>() :
						optional<posix_time::time_duration>(
							accessDirection == DEPARTURE_TO_ARRIVAL ?
							result.getEndTime().getSecondsDifference(originDateTime) :
							originDateTime.getSecondsDifference(result.getEndTime())
						)
				);

			}

			if(logger)
			{
				logger->close();
			}
		}



		RoutePlanner::SamePlacesException::SamePlacesException()
			: util::Exception("Same places in route planner")
		{

		}
	}
}
