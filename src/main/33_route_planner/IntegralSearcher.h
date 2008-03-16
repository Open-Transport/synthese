
/** IntegralSearcher class header.
	@file IntegralSearcher.h

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

#ifndef SYNTHESE_routeplanner_IntegralSearcher_h__
#define SYNTHESE_routeplanner_IntegralSearcher_h__

#include "01_util/Log.h"

#include "15_env/Types.h"

#include "33_route_planner/Types.h"
#include "33_route_planner/JourneysResult.h"

#include <map>
#include <list>
#include <utility>

namespace synthese
{
	namespace env
	{
		class Vertex;
		class VertexAccessMap;
		class Edge;
		class Journey;
	}

	namespace time
	{
		class DateTime;
	}

	namespace routeplanner
	{
		class BestVertexReachesMap;
		class JourneysResult;

		/** IntegralSearcher class.
			@ingroup m53

			This class provides an algorithm which returns all the vertices available in an area.

			The area is defined by several constraints.

			This algorithm can be used in a route planning : it handles optionally the concept of searched place (destination).

		*/
		class IntegralSearcher
		{
		private:
			typedef const env::Edge* (env::Edge::*PtrEdgeStep) () const;

			//! @name Parameters
			//@{
				const AccessParameters		_accessParameters;
				const AccessDirection		_accessDirection;
				const SearchAddresses		_searchAddresses;
				const SearchPhysicalStops	_searchPhysicalStops;
				const UseRoads				_useRoads;
				const UseLines				_useLines;
				JourneysResult&				_result;
				BestVertexReachesMap&		_bestVertexReachesMap;
				const env::VertexAccessMap&	_destinationVam;	//!< Can be a departure or an arrival, according to _accesDirection
				const time::DateTime&		_calculationTime;
				time::DateTime&				_minMaxDateTimeAtDestination;
				const int					_previousContinuousServiceDuration;
				const time::DateTime&		_previousContinuousServiceLastDeparture;
				const int					_maxDepth;
				const bool					_optim;
				const bool					_inverted;	//!< Indicates that the AccessDirection is the contraty to the planning order (2nd passe)
				std::ostream* const			_logStream;
				const util::Log::Level		_logLevel;
			//@}

			/** Integral search of objects within the network.
				@param vertices
				@param desiredTime Desired time.
				@param accessDirection
				@param currentJourney Journey currently being built.
				@param maxDepth Maximum recursion depth.
				@param accessDirection
				@param accessDirection
				@param searchAddresses Whether or not to search for addresses.
				@param searchPhysicalStops Whether or not to search for physicalStops.
				@param useRoads Filter : true = the search is allowed to use the road network
				@param useLines Filter : true = the search is allowed to use the transport network
				@param strictTime Must the departure time be strictly equal to desired time ?
			 */
			void _integralSearchRecursion (
				const env::VertexAccessMap& vertices
				, const time::DateTime& desiredTime
				, const env::Journey& currentJourney
				, int maxDepth
				, bool strictTime = false
			);

		public:
			IntegralSearcher(
				AccessDirection					accessDirection
				, const AccessParameters&		accessParameters
				, SearchAddresses				searchAddresses
				, SearchPhysicalStops			searchPhysicalStops
				, UseRoads						useRoads
				, UseLines						useLines
				, JourneysResult&				result
				, BestVertexReachesMap&			bestVertexReachesMap
				, const env::VertexAccessMap&	destinationVam
				, const time::DateTime&			calculationTime
				, time::DateTime&				minMaxDateTimeAtDestination
				, int							previousContinuousServiceDuration
				, const time::DateTime&			previousContinuousServiceLastDeparture
				, const int						maxDepth
				, bool							optim
				, bool							inverted
				, std::ostream* const			logStream = NULL
				, util::Log::Level				logLevel = util::Log::LEVEL_NONE
			);

			/** Launch of the integral search upon a vertex access map.
				@param result Result to consider and to write on
				@param vertices Access map
				@param desiredTime Time to use for starting the search
				@param maxDepth Maximum depth of recursion
				@param strictTime If true, only the journeys starting exactly at the desired time are selected, and the best vertex map use is in optimizing purpose
				@author Hugues Romain
				@date 2007				
			*/
			void integralSearch(
				const env::VertexAccessMap& vertices
				, const time::DateTime& desiredTime
				, bool strictTime = false
			);

			/** Launch of the integral search upon a journey corresponding to a beginning of a route planning.
				@param journey The journey to complete.
				@warning The journey must be non empty
				@author Hugues Romain
				@date 2007				
			*/
			void integralSearch(
				const env::Journey& journey
			);


			/** Journey utility evaluation.
				@param journey Journey to evaluate
				@param bool optimization mode
				@return pair<bool,bool> Possible values : false/false, true/true, false/true
					- first : utility to store the journey as result or a future result part.
					- second.first : utility to continue to traverse the rest of the path
					- second.second : utility to continue to iterate ont services of the same path
				@author Hugues Romain
				@date 2007				
			*/
			std::pair<bool,bool> evaluateJourney(
				const env::Journey& journey
				, bool optim
			) const;

		};
	}
}

#endif // SYNTHESE_routeplanner_IntegralSearcher_h__
