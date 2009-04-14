
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
#include "NamedPlace.h"

#include "DateTime.h"

namespace synthese
{
	using namespace env;
	using namespace geography;
	
	namespace routeplanner
	{
		/** JourneysResult class.
			@ingroup m53
		*/
		template<class JourneyComparator>
		class JourneysResult
		{			
		public:
			typedef std::set<graph::Journey*, JourneyComparator> ResultSet;

		private:
			typedef std::map<const graph::Vertex*, typename ResultSet::iterator> IndexMap;
			
			ResultSet	_result;
			IndexMap	_index;

		public:
			JourneysResult() {}



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
				void remove(const graph::Journey* journey)
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
						const graph::Journey* ptr = *its;
						_result.erase(its);
						_index.erase(it);
						delete ptr;
					}
				}



				/** Adds a journey to the result object.
					@param journey the journey to add
					@author Hugues Romain
				*/
				void add(graph::Journey* journey)
				{
					const graph::Vertex* vertex(journey->getEndEdge()->getFromVertex());
					remove(vertex);
					_index.insert(make_pair(vertex, _result.insert(journey).first));
				}

			
				
				/** Adds an empty journey for a specified vertex.
					@param method Access direction of the journey to create
					@author Hugues Romain
					@date 2008					
				*/
				void addEmptyJourney()
				{
					graph::Vertex* nullVertex(NULL);
					_index.insert(make_pair(nullVertex, _result.insert(new graph::Journey()).first));
				}



				/** Gets the first journey of the result set and remove it.
					@return Pointer to the first journey
					@warning The returned pointer must be deleted after use
				*/
				const graph::Journey* front()
				{
					assert(!empty());

					const graph::Journey* ptr(*_result.begin());
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
					bool updateMinSpeed
					, const time::DateTime& newMaxTime
					, const BestVertexReachesMap& bvrm
				){
					std::vector<graph::Journey*> journeysToAdd;
					std::vector<graph::Journey*> journeysToRemove;
					for (typename IndexMap::iterator it(_index.begin()); it != _index.end();)
					{
						graph::Journey* journey(*it->second);
						typename IndexMap::iterator next(it);
						++next;
						if (bvrm.mustBeCleared(it->first, journey->getEndTime(), newMaxTime))
							journeysToRemove.push_back(journey);
						else if (updateMinSpeed)
						{
							_result.erase(it->second);
							_index.erase(it);
							journey->setMinSpeedToEnd(newMaxTime);
							journeysToAdd.push_back(journey);
						}
						it = next;
					}
					BOOST_FOREACH(graph::Journey* journey, journeysToRemove)
					{
						remove(journey);
					}
					BOOST_FOREACH(graph::Journey* journey, journeysToAdd)
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
				const graph::Journey* const get(const graph::Vertex* vertex) const
				{
					typename IndexMap::const_iterator it(_index.find(vertex));
					if (it != _index.end())
					{
						typename ResultSet::const_iterator its(it->second);
						return *its;
					}
					else
						return NULL;
				}

				
				
				/** Is the result empty ?.
					@return bool true if the result is empty
					@author Hugues Romain
				*/
				bool empty() const
				{
					return _result.empty();
				}


				
				/** Log generator.
					@return std::string description of the journey for logging purposes
					@author Hugues Romain
				*/
				std::string	getLog() const
				{
					std::stringstream s;
					s	<< "<tr><th colspan=\"7\">Exploration queue (size=" << _result.size() << ")</th></tr>"
						<< "<tr><th>Place</th><th>Time</th><th>Score</th><th>Dist</th><th>Min spd</th><th>Dist.MinSpd</th><th>Place score</th></tr>"
						;
					BOOST_FOREACH(const graph::Journey* journey, _result)
					{
						if (journey->empty())
						{
							s << "<tr><td colspan=\"7\">Empty fake journey</td></tr>";
							continue;
						}

						s	<<
							"<tr><td>" <<
							dynamic_cast<const NamedPlace*>(
								journey->getEndEdge()->getHub()
							)->getFullName() <<
							"</td><td>" <<
							journey->getEndTime().toString() << "</td>"
							<< "<td>" << journey->getScore() << "</td>"
							<< "<td>" << journey->getSquareDistanceToEnd().getDistance() << "</td>"
							<< "<td>" << (journey->getSquareDistanceToEnd().getDistance() ? (0.06 * journey->getMinSpeedToEnd() / journey->getSquareDistanceToEnd().getDistance()) : -1) << "</td>"
							<< "<td>" << journey->getMinSpeedToEnd() << "</td>"
							<< "<td>" << journey->getEndEdge()->getHub()->getScore() << "</td>"
							<< "</tr>";
					}
					return s.str();
				}
			//@}
		};
	}
}

#endif // SYNTHESE_routeplanner_JourneysResult_h__
