
/** BestVertexReachesMap class header.
	@file BestVertexReachesMap.h

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

#ifndef SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H
#define SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H

#include "AlgorithmTypes.h"

#include <vector>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace graph
	{
		class Vertex;
		class VertexAccessMap;
	}

	namespace algorithm
	{
		class RoutePlanningIntermediateJourney;

		/** Best vertex reaches map class.
			@ingroup m33

			BestVertexReachesMap is not thread safe ! It is intended to be used only internally by a computing thread.

			The map stores for each vertex and each journey size the best time of vertex reach by number of transfers.

			The map helps to determinate :
				- if a journey ca be a source of recursion in the route planning process
				- if it is necessary to follow the current path to build other useful journeys

			The map stores for each goal vertex a list of best time used to reach the vertex with a minimal number of transfers.

			For example :
				- at least 2 transfers => best reach in 35 minutes
				- at least 3 transfers => best reach in 25 minutes

			Constant rule :
				- the reach duration for a greater number of transfer must be always shorter

			The way to determinate if a journey is useful is :
				- if the vertex record is empty, insert the journey and accept it
				- if the vertex is not empty, loop on the ascending transfer number. For each one :
					- if the recorded transfers number is less than the transfer number of the journey
						- if the recorded duration is smaller or equal to the duration of the journey, return not to keep the journey
						- if the recorded duration is longer than the duration of the journey, do nothing and continue the loop
					- if the recorded transfers number is equal to the transfers number of the journey :
						- if the recorded duration is smaller or equal to the duration of the journey, return not to keep the journey
						- if the recorded duration is longer than the duration of the journey, replace it by the value of the journey, delete all records with more transfers, and return to keep the journey
					- if the recorded transfers number is greater than the transfer number of the journey :
						- if the recorded duration is smaller than the duration of the journey, insert the values of the journey into the map and return to keep it
						- if the recorded duration is greater or equal to the duration of the journey, then insert the value of the journey, delete all records with more transfers including the current one, and return to keep the journey

			Examples :
			In the following map :
				- at least 2 transfers => best reach in 35 minutes
				- at least 3 transfers => best reach in 25 minutes

			Case : a 4 transfer journey with 30 minutes of duration
			Result : no update of the map, because of the duration of 25 minutes with only 3 transfers

			Case : a 2 transfer journey with 30 minutes of duration
			Result : replacement of the 2 transfers record :
				- at least 2 transfers => best reach in 30 minutes
				- at least 3 transfers => best reach in 25 minutes

			Case : a 4 transfer journey with 20 minutes of duration
			Result : insertion of the journey :
				- at least 2 transfers => best reach in 30 minutes
				- at least 3 transfers => best reach in 25 minutes
				- at least 4 transfers => best reach in 20 minutes
		*/
		class BestVertexReachesMap
		{
		 private:

			typedef std::vector<
				std::map<
					std::size_t,
					std::pair<
						boost::posix_time::time_duration,
						boost::shared_ptr<RoutePlanningIntermediateJourney>
			>	>	> TimeMap;

			TimeMap _bestTimeMap;
			const PlanningPhase _accessDirection;

			void _insert(
				TimeMap::value_type& vertexItem,
				std::size_t transfers,
				boost::posix_time::time_duration duration,
				const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey
			);

			void _insertAndPropagateInConnectionPlace(
				TimeMap::value_type& vertexItem,
				std::size_t transfers,
				boost::posix_time::time_duration duration,
				const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey,
				const graph::Vertex& vertex
			);

			void _removeDurationsForMoreTransfers(
				TimeMap::value_type& vertexItem,
				std::size_t transfers
			);

		public:
			BestVertexReachesMap(
				PlanningPhase accessDirection,
				const graph::VertexAccessMap& vam,
				const graph::VertexAccessMap& destinationVam,
				std::size_t vertexNumber
			);

			//! @name Query methods
			//@{
				/** Tests if the journey is useless according to the map.
					The methods auto-updates the map if the journey is useful.
					@param vertex Pointer to the reached vertex
					@param transferNumber Number of transfers of the journey
					@param duration Duration needed to reach the vertex
					@return true if the described journey is useful
				*/
				bool isUseLess(
					const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey,
					const boost::posix_time::ptime& originDateTime,
					bool propagateInConnectionPlace,
					bool strict = true
				);
			//@}
		};
}	}

#endif
