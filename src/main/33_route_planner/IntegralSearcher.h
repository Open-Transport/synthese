
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

#include "15_env/ServiceUse.h"
#include "15_env/Types.h"

#include "33_route_planner/Types.h"

#include <map>
#include <list>

namespace synthese
{
	namespace env
	{
		class Vertex;
		class VertexAccessMap;
		class Edge;
		class Journey;
	}

	namespace routeplanner
	{
		class BestVertexReachesMap;

		/** IntegralSearcher class.
			@ingroup m33

			This class provides an algorithm which returns all the vertices available in an area.

			The area is defined by several constraints.

			This algorithm can be used in a route planning : it handles optionally the concept of searched place (destination).

		*/
		class IntegralSearcher
		{
		private:
			typedef std::map<const env::Vertex*, env::Journey> IntegralSearchWorkingResult;
			
			typedef const env::Edge* (env::Edge::*PtrEdgeStep) () const;

			//! @name Parameters
			//@{
				const AccessParameters		_accessParameters;
				const AccessDirection		_accessDirection;
				const SearchAddresses		_searchAddresses;
				const SearchPhysicalStops	_searchPhysicalStops;
				const UseRoads				_useRoads;
				const UseLines				_useLines;
				BestVertexReachesMap&		_bestVertexReachesMap;
				const env::VertexAccessMap&	_destinationVam;	//!< Can be a departure or an arrival, according to _accesDirection
				const time::DateTime&		_calculationTime;
				time::DateTime&				_minMaxDateTimeAtDestination;
				const int					_previousContinuousServiceDuration;
				const time::DateTime&		_previousContinuousServiceLastDeparture;
			//@}

			//! @name Result
			//@{
				IntegralSearchWorkingResult	_result;
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

			bool _evaluateJourney(
				const env::Journey& journey
			) const;

		public:
			IntegralSearcher(
				const AccessDirection&			accessDirection
				, const AccessParameters&		accessParameters
				, const SearchAddresses&		searchAddresses
				, const SearchPhysicalStops&	searchPhysicalStops
				, const UseRoads&				useRoads
				, const UseLines&				useLines
				, BestVertexReachesMap&			bestVertexReachesMap
				, const env::VertexAccessMap&	destinationVam
				, const time::DateTime&			calculationTime
				, time::DateTime&				minMaxDateTimeAtDestination
				, int							previousContinuousServiceDuration
				, const time::DateTime&			previousContinuousServiceLastDeparture
				);

			env::Journeys integralSearch(
				const env::VertexAccessMap& vertices
				, const env::Journey& journey
				, const time::DateTime& desiredTime
				, int maxDepth
				, bool strictTime = false
				);
		};
	}
}

#endif // SYNTHESE_routeplanner_IntegralSearcher_h__
