
/** JourneysResult class implementation.
	@file JourneysResult.cpp

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

#include "JourneysResult.h"
#include "IntegralSearcher.h"

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
						it->first->getEndEdge().getFromVertex(),
						it
				)	);
			}
		}



		//! @name Getters
		//@{
			/** Result object getter.
				@return const ResultSet& the result
				@author Hugues Romain
			*/
			const JourneysResult::ResultSet& JourneysResult::getJourneys() const
			{
				return _result;
			}
		//@}



		//! @name Update methods
		//@{
			/** Removes the specified journey from the result object.
				@param journey the journey to remove
				@author Hugues Romain
			*/
			void JourneysResult::remove(boost::shared_ptr<RoutePlanningIntermediateJourney> journey)
			{
				const graph::Vertex* vertex(journey->getEndEdge().getFromVertex());
				remove(vertex);
			}



			void JourneysResult::remove(const graph::Vertex* vertex)
			{
				IndexMap::iterator it(_index.find(vertex));
				if (it != _index.end())
				{
					ResultSet::iterator its(it->second);
					_result.erase(its);
					_index.erase(it);
				}
			}



			/** Adds a journey to the result object.
				@param journey the journey to add
				@author Hugues Romain
			*/
			void JourneysResult::add(boost::shared_ptr<RoutePlanningIntermediateJourney> journey)
			{
				const graph::Vertex* vertex(journey->getEndEdge().getFromVertex());
				boost::posix_time::time_duration duration(
					_accessDirection == DEPARTURE_TO_ARRIVAL ?
					journey->getEndTime() - _originDateTime :
					_originDateTime - journey->getEndTime()
				);
				remove(vertex);
				_index.insert(
					std::make_pair(
						vertex,
						_result.insert(
							std::make_pair(journey, duration)
						).first
				)	);
			}

		
			
			/** Adds an empty journey for a specified vertex.
				@param method Access direction of the journey to create
				@author Hugues Romain
				@date 2008					
			*/
			void JourneysResult::addEmptyJourney()
			{
				graph::Vertex* nullVertex(NULL);
				_index.insert(
					std::make_pair(
						nullVertex,
						_result.insert(
							std::make_pair(
								boost::shared_ptr<RoutePlanningIntermediateJourney>(new RoutePlanningIntermediateJourney(_accessDirection)),
								boost::posix_time::minutes(0)
						)	).first
				)	);
			}



			/** Gets the first journey of the result set and remove it.
				@return Pointer to the first journey
				@warning The returned pointer must be deleted after use
			*/
			boost::shared_ptr<RoutePlanningIntermediateJourney> JourneysResult::front()
			{
				assert(!empty());

				boost::shared_ptr<RoutePlanningIntermediateJourney> ptr(_result.begin()->first);
				_index.erase(ptr->empty() ? NULL : ptr->getEndEdge().getFromVertex());
				_result.erase(_result.begin());
				return ptr;
			}

			
			
			/** Removes useless result journeys according to the current best reaches.
				@param updateMinSpeed
				@param newMaxTime
				@param bvrm the best reaches map to control
				@author Hugues Romain
			*/
			void JourneysResult::cleanup(
				bool updateMinSpeed,
				const ptime& newMaxTime,
				BestVertexReachesMap& bvrm,
				bool propagateInConnectionPlace,
				bool strict,
				const IntegralSearcher& is
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
					if(	_accessDirection == DEPARTURE_TO_ARRIVAL && journey->getEndTime() >= newMaxTime ||
						_accessDirection == ARRIVAL_TO_DEPARTURE && journey->getEndTime() <= newMaxTime
						// Add reach ability test
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
				BOOST_FOREACH(boost::shared_ptr<RoutePlanningIntermediateJourney> journey, journeysToRemove)
				{
					remove(journey);
				}
				BOOST_FOREACH(boost::shared_ptr<RoutePlanningIntermediateJourney> journey, journeysToAdd)
				{
					add(journey);
				}
			}
		//@}



		//! @name Queries
		//@{
			/** Gets the result journey that reaches the specified vertex.
				@param vertex Vertex to be reached by the returned result journey
				@return const pt::Journey* const The result journey that reaches the specified vertex
				@author Hugues Romain
			*/
			boost::shared_ptr<RoutePlanningIntermediateJourney> JourneysResult::get(const graph::Vertex* vertex) const
			{
				IndexMap::const_iterator it(_index.find(vertex));
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
		//@}

	}
}
