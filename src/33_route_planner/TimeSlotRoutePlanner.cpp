
/** TimeSlotRoutePlanner class implementation.
	@file TimeSlotRoutePlanner.cpp

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

#include "TimeSlotRoutePlanner.h"
#include "RoutePlanner.h"

#include <boost/foreach.hpp>
#include <sstream>
#include <boost/date_time/posix_time/posix_time_io.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace graph;
	using namespace util;

	namespace algorithm
	{
		TimeSlotRoutePlanner::TimeSlotRoutePlanner(
			const graph::VertexAccessMap& originVam,
			const graph::VertexAccessMap& destinationVam,
			const ptime& lowerDepartureTime,
			const ptime& higherDepartureTime,
			const ptime& lowerArrivalTime,
			const ptime& higherArrivalTime,
			const graph::GraphIdType			whatToSearch,
			const graph::GraphIdType			graphToUse,
			optional<posix_time::time_duration> maxDuration,
			optional<size_t> maxSolutionsNumber,
			AccessParameters accessParameters,
			PlanningOrder planningOrder,
			std::ostream* logStream
		):	_originVam(originVam),
			_destinationVam(destinationVam),
			_lowestDepartureTime(lowerDepartureTime),
			_highestDepartureTime(higherDepartureTime),
			_lowestArrivalTime(lowerArrivalTime),
			_highestArrivalTime(higherArrivalTime),
			_whatToSearch(whatToSearch),
			_graphToUse(graphToUse),
			_maxDuration(maxDuration),
			_maxSolutionsNumber(maxSolutionsNumber),
			_accessParameters(accessParameters),
			_planningOrder(planningOrder),
			_logStream(logStream),
			_journeyTemplates(graphToUse)
		{
		}



		TimeSlotRoutePlanner::TimeSlotRoutePlanner(
			const graph::VertexAccessMap& originVam,
			const graph::VertexAccessMap& destinationVam,
			const Result::value_type& continuousService,
			const graph::GraphIdType			whatToSearch,
			const graph::GraphIdType			graphToUse,
			optional<boost::posix_time::time_duration> maxDuration,
			optional<std::size_t>	maxSolutionsNumber,
			AccessParameters accessParameters,
			const PlanningOrder planningOrder,
			std::ostream* logStream
		):	_originVam(originVam),
			_destinationVam(destinationVam),
			_lowestDepartureTime(continuousService.getFirstDepartureTime()),
			_highestDepartureTime(continuousService.getLastDepartureTime()),
			_lowestArrivalTime(continuousService.getFirstArrivalTime()),
			_highestArrivalTime(continuousService.getLastArrivalTime()),
			_whatToSearch(whatToSearch),
			_graphToUse(graphToUse),
			_maxDuration(
				(!maxDuration || continuousService.getDuration() - posix_time::seconds(1) < *maxDuration) ?
				continuousService.getDuration() - posix_time::seconds(1) :
				maxDuration
			),
			_maxSolutionsNumber(maxSolutionsNumber),
			_accessParameters(),
			_planningOrder(planningOrder),
			_logStream(logStream),
			_parentContinuousService(continuousService),
			_journeyTemplates(graphToUse)
		{
		}



		const ptime& TimeSlotRoutePlanner::getLowestDepartureTime() const
		{
			return _lowestDepartureTime;
		}



		const ptime& TimeSlotRoutePlanner::getHighestDepartureTime() const
		{
			return _highestDepartureTime;
		}



		const ptime& TimeSlotRoutePlanner::getLowestArrivalTime() const
		{
			return _lowestArrivalTime;
		}



		const ptime& TimeSlotRoutePlanner::getHighestArrivalTime() const
		{
			return _highestArrivalTime;
		}

		TimeSlotRoutePlanner::Result TimeSlotRoutePlanner::run()
		{
			Result result;

			// Time loop
			for(ptime originDateTime(_planningOrder == DEPARTURE_FIRST ? _lowestDepartureTime : _highestArrivalTime);
				_planningOrder == DEPARTURE_FIRST ? originDateTime <= _highestDepartureTime : originDateTime >= _lowestArrivalTime;
				_planningOrder == DEPARTURE_FIRST ? originDateTime += minutes(1) : originDateTime -= minutes(1)
			){
				if(	_logStream
				){
					*_logStream << "<h2>Route planning " << (result.size()+1) << " at " << _lowestDepartureTime << "</h2>";
				}

				RoutePlanner r(
					_originVam,
					_destinationVam,
					_planningOrder,
					_accessParameters,
					_maxDuration,
					originDateTime,
					_planningOrder == DEPARTURE_FIRST ? _highestDepartureTime : _lowestArrivalTime,
					_planningOrder == DEPARTURE_FIRST ? _highestArrivalTime : _lowestDepartureTime,
					_whatToSearch,
					_graphToUse,
					_logStream,
					_journeyTemplates
				);
				Journey journey(r.run());

				if(journey.empty()) break;

				//! <li> If the journey is continuous, attemt to break it. </li>
				if(	journey.getContinuousServiceRange ().total_seconds() > 60)
				{
					TimeSlotRoutePlanner tsr(
						_originVam,
						_destinationVam,
						journey,
						_whatToSearch,
						_graphToUse,
						_maxDuration,
						_maxSolutionsNumber ? *_maxSolutionsNumber - result.size() : _maxSolutionsNumber,
						_accessParameters,
						_planningOrder,
						_logStream
					);
					Result subResult(_MergeSubResultAndParentContinuousService(journey, tsr.run()));
			
					if(_planningOrder == DEPARTURE_FIRST)
					{
						BOOST_FOREACH(const Result::value_type& sj, subResult)
						{
							result.push_back(sj);
							_journeyTemplates.addResult(journey);
						}
					}
					else
					{
						for(Result::const_reverse_iterator it(subResult.rbegin()); it != subResult.rend(); ++it)
						{
							result.push_front(*it);
							_journeyTemplates.addResult(journey);
						}
					}
				}
				else
				{
					if(_planningOrder == DEPARTURE_FIRST)
					{
						// Verifiy that the journey is not the same continuous service than the last one.
						// If yes, enlarge the time slot of the existing continuous service
						result.push_back(journey);
						_journeyTemplates.addResult(journey);
					}
					else
					{
						// Verifiy that the journey is not the same continuous service than the first one.
						// If yes, enlarge the time slot of the existing continuous service and shift it
						result.push_front(journey);
						_journeyTemplates.addResult(journey);
					}

				}

				// Replace 1 minute wide continous service by two scheduled services
				if(!result.empty() && result.back().getContinuousServiceRange().total_seconds() == 60)
				{
					// TODO
				}

				if(_maxSolutionsNumber && result.size() >= *_maxSolutionsNumber)
				{
					while(result.size() > *_maxSolutionsNumber)
					{
						if(_planningOrder == DEPARTURE_FIRST)
						{
							result.pop_back();
						}
						else
						{
							result.pop_front();
						}
					}
					break;
				}

				if(!result.empty())
				{
					if(_planningOrder == DEPARTURE_FIRST)
					{
						const Journey& last(result.back());
						originDateTime = last.getLastDepartureTime();
					}
					else
					{
						const Journey& first(result.front());
						originDateTime = first.getFirstArrivalTime();
					}
				}
			}

			return result;
		}



		TimeSlotRoutePlanner::Result TimeSlotRoutePlanner::_MergeSubResultAndParentContinuousService(
			const TimeSlotRoutePlanner::Result::value_type& parentContinuousService,
			const TimeSlotRoutePlanner::Result& subResult
		){
			Result result;

			if(subResult.empty())
			{
				result.push_back(parentContinuousService);
				return result;
			}

			ptime departureTime(parentContinuousService.getFirstDepartureTime());
			BOOST_FOREACH(const Journey& subJourney, subResult)
			{
				// Insertion of a journey of the parent continuous service before
				ptime precedingLastDepartureTime(subJourney.getFirstArrivalTime());
				precedingLastDepartureTime -= days(1);
				precedingLastDepartureTime -= parentContinuousService.getDuration();
				if(precedingLastDepartureTime > departureTime)
				{
					posix_time::time_duration toShift(
						departureTime - parentContinuousService.getFirstDepartureTime()
					);
					TimeSlotRoutePlanner::Result::value_type j(parentContinuousService);
					BOOST_FOREACH(Journey::ServiceUses::value_type& leg, j.getServiceUses())
					{
						leg.shift(toShift);
					}
					j.setContinuousServiceRange(precedingLastDepartureTime - departureTime);
					result.push_back(j);
				}

				result.push_back(subJourney);

				departureTime = subJourney.getFirstDepartureTime();
				departureTime += days(1);
			}

			ptime lastDepartureTime(parentContinuousService.getLastDepartureTime());
			if(departureTime <= lastDepartureTime)
			{
				posix_time::time_duration toShift(
					departureTime - parentContinuousService.getFirstDepartureTime()
				);
				TimeSlotRoutePlanner::Result::value_type j(parentContinuousService);
				BOOST_FOREACH(Journey::ServiceUses::value_type& leg, j.getServiceUses())
				{
					leg.shift(toShift);
				}
				j.setContinuousServiceRange(lastDepartureTime - departureTime);
				result.push_back(j);
			}

			return result;
		}
	}
}
