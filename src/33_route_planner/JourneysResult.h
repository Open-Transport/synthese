
/** JourneysResult class header.
	@file JourneysResult.h

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

#ifndef SYNTHESE_routeplanner_JourneysResult_h__
#define SYNTHESE_routeplanner_JourneysResult_h__

#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <boost/foreach.hpp>

#include "BestVertexReachesMap.h"

#include "Journey.h"
#include "Edge.h"
#include "Vertex.h"
#include "Hub.h"
#include "GraphTypes.h"

#include "DateTime.h"

namespace synthese
{
	namespace algorithm
	{
		/** List of journeys that should be part of the result of a routing process.
			@ingroup m53
		*/
		template<class JourneyComparator>
		class JourneysResult
		{			
		public:
			typedef std::map<
				boost::shared_ptr<graph::Journey>,
				boost::posix_time::time_duration,
				JourneyComparator
			> ResultSet;

		private:
			typedef std::map<const graph::Vertex*, typename ResultSet::iterator> IndexMap;
			
			time::DateTime _originDateTime;
			boost::optional<graph::AccessDirection> _accessDirection;
			ResultSet	_result;
			IndexMap	_index;

		public:
			JourneysResult(
				const time::DateTime& originDateTime
			):	_originDateTime(originDateTime)
			{}


			void operator=(const JourneysResult<JourneyComparator>& other)
			{
				_originDateTime = other._originDateTime;
				_accessDirection = other._accessDirection;
				_result = other._result;
				for(typename ResultSet::iterator it(_result.begin()); it != _result.end(); ++it)
				{
					_index.insert(
						std::make_pair(
							it->first->getEndEdge()->getFromVertex(),
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
				const ResultSet& getJourneys() const
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
				void remove(boost::shared_ptr<graph::Journey> journey)
				{
					const graph::Vertex* vertex(journey->getEndEdge()->getFromVertex());
					remove(vertex);
				}



				void remove(const graph::Vertex* vertex)
				{
					typename IndexMap::iterator it(_index.find(vertex));
					if (it != _index.end())
					{
						typename ResultSet::iterator its(it->second);
						_result.erase(its);
						_index.erase(it);
					}
				}



				/** Adds a journey to the result object.
					@param journey the journey to add
					@author Hugues Romain
				*/
				void add(boost::shared_ptr<graph::Journey> journey)
				{
					if(!_accessDirection) _accessDirection = journey->getMethod();
					assert(*_accessDirection == journey->getMethod());

					const graph::Vertex* vertex(journey->getEndEdge()->getFromVertex());
					boost::posix_time::time_duration duration(
						_accessDirection == graph::DEPARTURE_TO_ARRIVAL ?
						journey->getEndTime().getSecondsDifference(_originDateTime) :
						_originDateTime.getSecondsDifference(journey->getEndTime())
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
				void addEmptyJourney()
				{
					graph::Vertex* nullVertex(NULL);
					_index.insert(
						std::make_pair(
							nullVertex,
							_result.insert(
								std::make_pair(
									boost::shared_ptr<graph::Journey>(new graph::Journey),
									boost::posix_time::minutes(0)
							)	).first
					)	);
				}



				/** Gets the first journey of the result set and remove it.
					@return Pointer to the first journey
					@warning The returned pointer must be deleted after use
				*/
				boost::shared_ptr<graph::Journey> front()
				{
					assert(!empty());

					boost::shared_ptr<graph::Journey> ptr(_result.begin()->first);
					_index.erase(ptr->empty() ? NULL : ptr->getEndEdge()->getFromVertex());
					_result.erase(_result.begin());
					return ptr;
				}

				
				
				/** Removes useless result journeys according to the current best reaches.
					@param updateMinSpeed
					@param newMaxTime
					@param bvrm the best reaches map to control
					@author Hugues Romain
				*/
				void cleanup(
					bool updateMinSpeed,
					const time::DateTime& newMaxTime,
					BestVertexReachesMap& bvrm,
					bool propagateInConnectionPlace,
					const time::DateTime& originDateTime,
					bool strict,
					int totalDistance
				){
					std::vector<boost::shared_ptr<graph::Journey> > journeysToAdd;
					std::vector<boost::shared_ptr<graph::Journey> > journeysToRemove;
					for (typename IndexMap::iterator it(_index.begin()); it != _index.end();)
					{
						boost::shared_ptr<graph::Journey> journey(it->second->first);
						typename IndexMap::iterator next(it);
						++next;
						if(	journey->getMethod() == graph::DEPARTURE_TO_ARRIVAL && journey->getEndTime() >= newMaxTime ||
							journey->getMethod() == graph::ARRIVAL_TO_DEPARTURE && journey->getEndTime() <= newMaxTime
							// Add reach ability test
						){
							journeysToRemove.push_back(journey);
						}
						else if (updateMinSpeed)
						{
							_result.erase(it->second);
							_index.erase(it);
							journey->setMinSpeedToEnd(
								originDateTime,
								journey->getMethod() == graph::DEPARTURE_TO_ARRIVAL ? newMaxTime.getSecondsDifference(originDateTime) : originDateTime.getSecondsDifference(newMaxTime), totalDistance);
							journeysToAdd.push_back(journey);
						}
						it = next;
					}
					BOOST_FOREACH(boost::shared_ptr<graph::Journey> journey, journeysToRemove)
					{
						remove(journey);
					}
					BOOST_FOREACH(boost::shared_ptr<graph::Journey> journey, journeysToAdd)
					{
						add(journey);
					}
				}
			//@}



			//! @name Queries
			//@{
				/** Gets the result journey that reaches the specified vertex.
					@param vertex Vertex to be reached by the returned result journey
					@return const env::Journey* const The result journey that reaches the specified vertex
					@author Hugues Romain
				*/
				boost::shared_ptr<graph::Journey> get(const graph::Vertex* vertex) const
				{
					typename IndexMap::const_iterator it(_index.find(vertex));
					if (it != _index.end())
					{
						typename ResultSet::const_iterator its(it->second);
						return its->first;
					}
					else
						return boost::shared_ptr<graph::Journey>();
				}

				
				
				/** Is the result empty ?.
					@return bool true if the result is empty
					@author Hugues Romain
				*/
				bool empty() const
				{
					return _result.empty();
				}
			//@}
		};
	}
}

#endif // SYNTHESE_routeplanner_JourneysResult_h__
