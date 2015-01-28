
/** VAMConverter class header.
	@file VAMConverter.hpp

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

#ifndef SYNTHESE_algorithm_VAMConverter_hpp__
#define SYNTHESE_algorithm_VAMConverter_hpp__

#include "AccessParameters.h"
#include "AlgorithmTypes.h"
#include "VertexAccessMap.h"

namespace synthese
{
	namespace geography
	{
		class Place;
	}
	namespace algorithm
	{
		class AlgorithmLogger;
		
		/** VAMConverter class.
			@ingroup m33
		*/
		class VAMConverter
		{
		private:
			const graph::AccessParameters		_accessParameters;
			const AlgorithmLogger& _logger;
			const graph::GraphIdType _whatToSearch;
			const graph::GraphIdType _graphToUse;
			const boost::posix_time::ptime& _lowestDepartureTime;
			const boost::posix_time::ptime& _highestDepartureTime;
			const boost::posix_time::ptime& _lowestArrivalTime;
			const boost::posix_time::ptime& _highestArrivalTime;
			const geography::Place* _origin;
			const geography::Place* _destination;

		public:
			VAMConverter(
				const graph::AccessParameters& accessParameters,
				const AlgorithmLogger& logger,
				const graph::GraphIdType whatToSearch,
				const graph::GraphIdType graphToUse,
				const boost::posix_time::ptime& lowestDepartureTime,
				const boost::posix_time::ptime& highestDepartureTime,
				const boost::posix_time::ptime& lowestArrivalTime,
				const boost::posix_time::ptime& highestArrivalTime,
				const geography::Place* origin,
				const geography::Place* destination
			);



			//////////////////////////////////////////////////////////////////////////
			/// Search of all physical stops reachable from the departure or the
			/// arrival place.
			/// @param vam the points to start the search from
			/// @param destinationVam the points to reach if possible
			/// @param direction the search is for departure or arrival places
			/// @return the list of reachable stops with the duration of approach,
			/// containing the departure and arrival places with empty approach, and
			/// other stops with non empty approach, using the road network to reach them.
			/// @author Hugues Romain
			graph::VertexAccessMap run(
				const graph::VertexAccessMap& vam,
				const graph::VertexAccessMap& destinationVam,
				algorithm::PlanningPhase direction
			) const;
		};
	}
}

#endif // SYNTHESE_algorithm_VAMConverter_hpp__

