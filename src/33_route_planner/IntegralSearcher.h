
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

#include "Log.h"

#include "15_env/Types.h"
#include "AccessParameters.h"

#include "33_route_planner/Types.h"
#include "JourneysResult.h"

#include <map>
#include <list>
#include <utility>

namespace synthese
{
	namespace graph
	{
		class Vertex;
		class VertexAccessMap;
		class Edge;
		class Journey;
		class JourneyComparator;
	}

	namespace time
	{
		class DateTime;
	}

	namespace routeplanner
	{
		class BestVertexReachesMap;

		/** IntegralSearcher class.
			@ingroup m53

			This class provides an algorithm which returns all the vertices available in an area.

			The area is defined by several constraints.

			This algorithm can be used in a route planning : it handles optionally the concept of searched place (destination).
		*/
		class IntegralSearcher
		{
		private:
			typedef const graph::Edge* (graph::Edge::*PtrEdgeStep) () const;

			class _JourneyComparator
			{
			public:
				bool operator()(const graph::Journey* j1, const graph::Journey* j2) const;
			};

			//! @name Parameters
			//@{
				const env::AccessParameters				_accessParameters;
				const graph::AccessDirection			_accessDirection;
				const SearchAddresses					_searchAddresses;
				const SearchPhysicalStops				_searchPhysicalStops;
				const UseRoads							_useRoads;
				const UseLines							_useLines;
				JourneysResult<graph::JourneyComparator>&	_result;
				BestVertexReachesMap&					_bestVertexReachesMap;
				const graph::VertexAccessMap&			_destinationVam;	//!< Can be a departure or an arrival, according to _accesDirection
				const time::DateTime&					_calculationTime;
				time::DateTime&							_minMaxDateTimeAtDestination;
				const int								_previousContinuousServiceDuration;
				const time::DateTime&					_previousContinuousServiceLastDeparture;
				const int								_maxDepth;
				const bool								_optim;
				const bool								_inverted;	//!< Indicates that the AccessDirection is the contraty to the planning order (2nd passe)
				std::ostream* const						_logStream;
				const util::Log::Level					_logLevel;
			//@}


		public:
			IntegralSearcher(
				graph::AccessDirection			accessDirection
				, const env::AccessParameters&	accessParameters
				, SearchAddresses				searchAddresses
				, SearchPhysicalStops			searchPhysicalStops
				, UseRoads						useRoads
				, UseLines									useLines
				, JourneysResult<graph::JourneyComparator>&	result
				, BestVertexReachesMap&			bestVertexReachesMap
				, const graph::VertexAccessMap&	destinationVam
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


			/** Integral search of objects within the network.
				@param vertices
				@param desiredTime Desired time.
				@param currentJourney Journey currently being built.
				@param maxDepth Maximum recursion depth.
				@param strictTime Must the departure time be strictly equal to desired time ?
			 */
			void integralSearch(
				const graph::VertexAccessMap& vertices
				, const time::DateTime& desiredTime
				, const graph::Journey& currentJourney
				, int maxDepth
				, bool strictTime = false
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
				const graph::Journey& journey
				, bool optim
			) const;

		};
	}
}

#endif // SYNTHESE_routeplanner_IntegralSearcher_h__
