
/** JourneysResult class implementation.
	@file JourneysResult.cpp

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

#include "JourneysResult.h"

#include "Edge.h"
#include "Hub.h"
#include "IntegralSearcher.h"
#include "Vertex.h"

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace algorithm
	{
		void JourneysResult::operator=(const JourneysResult& other)
		{
			assert(_accessDirection == other._accessDirection);

			_originDateTime = other._originDateTime;
			_result = other._result;
			for(ResultSet::iterator it(_result.begin()); it != _result.end(); ++it)
			{
				_index.insert(
					std::make_pair(
						std::make_pair(
							it->first->getEndEdge().getFromVertex(),
							//it->first->getEndTime() // TODO : replace with constant date time object = 00:00
							boost::posix_time::ptime()
						),
						it
				)	);
			}
		}



		const JourneysResult::ResultSet& JourneysResult::getJourneys() const
		{
			return _result;
		}



		void JourneysResult::remove(
			const RoutePlanningIntermediateJourney& journey
		){
			const graph::Vertex* vertex(journey.getEndEdge().getFromVertex());
			//boost::posix_time::ptime arrivalDateTime = journey.getEndTime();
			boost::posix_time::ptime arrivalDateTime;
			remove(vertex, arrivalDateTime);
		}



		void JourneysResult::remove(const graph::Vertex* vertex, boost::posix_time::ptime arrivalDateTime)
		{
			IndexMap::iterator it(_index.find(std::make_pair(vertex, arrivalDateTime)));
			if (it != _index.end())
			{
				ResultSet::iterator its(it->second);
				_result.erase(its);
				_index.erase(it);
			}
		}



		void JourneysResult::add(
			const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey
		){
			const graph::Vertex* vertex(journey->getEndEdge().getFromVertex());
			boost::posix_time::time_duration duration(
				_accessDirection == DEPARTURE_TO_ARRIVAL ?
				journey->getEndTime() - _originDateTime :
				_originDateTime - journey->getEndTime()
			);
			//remove(vertex, journey->getEndTime());
			remove(vertex, boost::posix_time::ptime());
			_index.insert(
				std::make_pair(
					std::make_pair(
						vertex,
						//journey->getEndTime() // TODO : replace with constant date time object = 00:00
						boost::posix_time::ptime()
					),
					_result.insert(
						std::make_pair(journey, duration)
					).first
			)	);
		}



		void JourneysResult::addEmptyJourney()
		{
			graph::Vertex* nullVertex(NULL);
			_index.insert(
				std::make_pair(
					std::make_pair(
						nullVertex,
						ptime()
					),
					_result.insert(
						std::make_pair(
							boost::shared_ptr<RoutePlanningIntermediateJourney>(new RoutePlanningIntermediateJourney(_accessDirection)),
							boost::posix_time::minutes(0)
					)	).first
			)	);
		}



		boost::shared_ptr<RoutePlanningIntermediateJourney> JourneysResult::front()
		{
			assert(!empty());

			boost::shared_ptr<RoutePlanningIntermediateJourney> ptr(_result.begin()->first);
			//_index.erase(std::make_pair(ptr->empty() ? NULL : ptr->getEndEdge().getFromVertex(), ptr->empty() ? boost::gregorian::not_a_date_time : ptr->getEndTime()));
			_index.erase(std::make_pair(ptr->empty() ? NULL : ptr->getEndEdge().getFromVertex(), ptime()));
			_result.erase(_result.begin());
			return ptr;
		}



		void JourneysResult::cleanup(
			bool updateMinSpeed,
			const ptime& newMaxTime,
			BestVertexReachesMap& bvrm,
			bool propagateInConnectionPlace,
			bool strict,
			const IntegralSearcher& is,
			double vmax
		){
			std::vector<boost::shared_ptr<RoutePlanningIntermediateJourney> > journeysToAdd;
			std::vector<boost::shared_ptr<RoutePlanningIntermediateJourney> > journeysToRemove;

			time_duration newTotalDuration(
				_accessDirection == DEPARTURE_TO_ARRIVAL ?
				newMaxTime - is.getOriginDateTime() :
				is.getOriginDateTime() - newMaxTime
			);

			for (IndexMap::iterator it(_index.begin()); it != _index.end();)
			{
				boost::shared_ptr<RoutePlanningIntermediateJourney> journey(it->second->first);
				IndexMap::iterator next(it);
				++next;

				ptime bestHopedGoalAccessDateTime (journey->getEndTime(false));
				time_duration bestTransferDelay(
					journey->getEndEdge().getFromVertex()->getHub()->getMinTransferDelay()
				);
				if(_accessDirection == DEPARTURE_TO_ARRIVAL)
				{
					bestHopedGoalAccessDateTime += bestTransferDelay;	// Minimal time to transfer
				}
				else
				{
					bestHopedGoalAccessDateTime -= bestTransferDelay;	// Minimal time to transfer
				}
				if(journey->getDistanceToEnd() && *journey->getDistanceToEnd() > 5000)
				{
					time_duration bestPossibleDuration(
						seconds(static_cast<long>(*journey->getDistanceToEnd() / vmax))
					);
					if(_accessDirection == DEPARTURE_TO_ARRIVAL)
					{
						bestHopedGoalAccessDateTime	+= bestPossibleDuration;	// Minimal time to reach the goal
					}
					else
					{
						bestHopedGoalAccessDateTime	-= bestPossibleDuration;	// Minimal time to reach the goal
					}
				}
				if(	(_accessDirection == DEPARTURE_TO_ARRIVAL && bestHopedGoalAccessDateTime >= newMaxTime) ||
					(_accessDirection == ARRIVAL_TO_DEPARTURE && bestHopedGoalAccessDateTime <= newMaxTime)
				){
					journeysToRemove.push_back(journey);
				}
				else if (updateMinSpeed)
				{
					_result.erase(it->second);
					_index.erase(it);
					journey->setScore(
						is._getScore(
							newTotalDuration,
							*journey->getDistanceToEnd(),
							_accessDirection == DEPARTURE_TO_ARRIVAL ?
								journey->getFirstArrivalTime() - is.getOriginDateTime() :
								is.getOriginDateTime() - journey->getLastDepartureTime(),
							*(	_accessDirection == DEPARTURE_TO_ARRIVAL ?
								journey->getDestination():
								journey->getOrigin()
							)->getFromVertex()->getHub()
					)	);
					journeysToAdd.push_back(journey);
				}
				it = next;
			}
			BOOST_FOREACH(const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey, journeysToRemove)
			{
				remove(*journey);
			}
			BOOST_FOREACH(const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey, journeysToAdd)
			{
				add(journey);
			}
		}



		boost::shared_ptr<RoutePlanningIntermediateJourney> JourneysResult::get(const graph::Vertex* vertex, boost::posix_time::ptime arrivalDateTime) const
		{
			//IndexMap::const_iterator it(_index.find(std::make_pair(vertex, arrivalDateTime)));
			IndexMap::const_iterator it(_index.find(std::make_pair(vertex, ptime())));
			if (it != _index.end())
			{
				ResultSet::const_iterator its(it->second);
				return its->first;
			}
			else
				return boost::shared_ptr<RoutePlanningIntermediateJourney>();
		}



		/** Is the result empty ?.
			@return bool true if the result is empty
			@author Hugues Romain
		*/
		bool JourneysResult::empty() const
		{
			return _result.empty();
		}
}	}
