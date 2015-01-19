
/** JourneysResult class header.
	@file JourneysResult.h

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

#ifndef SYNTHESE_routeplanner_JourneysResult_h__
#define SYNTHESE_routeplanner_JourneysResult_h__

#include "BestVertexReachesMap.h"
#include "GraphTypes.h"
#include "RoutePlanningIntermediateJourney.hpp"

#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace graph
	{
		class Vertex;
	}

	namespace algorithm
	{
		class IntegralSearcher;

		/** List of journeys that should be part of the result of a routing process.
			@ingroup m53
		*/
		class JourneysResult
		{
		public:
			typedef std::map<
				boost::shared_ptr<RoutePlanningIntermediateJourney>,
				boost::posix_time::time_duration,
				RoutePlanningIntermediateJourney::Comparator
			> ResultSet;

		private:
			typedef std::map<
				std::pair<
					const graph::Vertex*, 
					boost::posix_time::ptime
				>, ResultSet::iterator
			> IndexMap;

			boost::posix_time::ptime _originDateTime;
			const PlanningPhase _accessDirection;
			ResultSet	_result;
			IndexMap	_index;

		public:
			JourneysResult(
				const boost::posix_time::ptime& originDateTime,
				PlanningPhase planningPhase
			):	_originDateTime(originDateTime),
				_accessDirection(planningPhase)
			{}


			//////////////////////////////////////////////////////////////////////////
			/// @pre planning phase of the two objects must be the same
			void operator=(const JourneysResult& other);



			//! @name Getters
			//@{
				/** Result object getter.
					@return const ResultSet& the result
					@author Hugues Romain
				*/
				const ResultSet& getJourneys() const;
			//@}



			//! @name Update methods
			//@{
				/** Removes the specified journey from the result object.
					@param journey the journey to remove
					@author Hugues Romain
				*/
				void remove(
					const RoutePlanningIntermediateJourney& journey
				);



				void remove(const graph::Vertex* vertex, boost::posix_time::ptime arrivalDateTime);



				/** Adds a journey to the result object.
					@param journey the journey to add
					@author Hugues Romain
				*/
				void add(
					const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey
				);



				/** Adds an empty journey for a specified vertex.
					@author Hugues Romain
					@date 2008
				*/
				void addEmptyJourney();



				/** Gets the first journey of the result set and remove it.
					@return Pointer to the first journey
					@warning The returned pointer must be deleted after use
				*/
				boost::shared_ptr<RoutePlanningIntermediateJourney> front();



				/** Removes useless result journeys according to the current best reaches.
					@param updateMinSpeed
					@param newMaxTime
					@param bvrm the best reaches map to control
					@author Hugues Romain
				*/
				void cleanup(
					bool updateMinSpeed,
					const boost::posix_time::ptime& newMaxTime,
					BestVertexReachesMap& bvrm,
					bool propagateInConnectionPlace,
					bool strict,
					const IntegralSearcher& is,
					double vmax
				);
			//@}



			//! @name Queries
			//@{
				/** Gets the result journey that reaches the specified vertex.
					@param vertex Vertex to be reached by the returned result journey
					@param arrivalDateTime Arrival date to be reached by the returned result journey
					@return const pt::Journey* const The result journey that reaches the specified vertex
					@author Hugues Romain
				*/
				boost::shared_ptr<RoutePlanningIntermediateJourney> get(const graph::Vertex* vertex, boost::posix_time::ptime arrivalDateTime) const;



				/** Is the result empty ?.
					@return bool true if the result is empty
					@author Hugues Romain
				*/
				bool empty() const;
			//@}
		};
}	}

#endif // SYNTHESE_routeplanner_JourneysResult_h__
