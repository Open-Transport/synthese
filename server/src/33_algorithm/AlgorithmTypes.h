
/** AlgorithmTypes class header.
	@file AlgorithmTypes.h

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

#ifndef SYNTHESE_routeplanner_RoutePlannerTypes_h__
#define SYNTHESE_routeplanner_RoutePlannerTypes_h__

namespace synthese
{
	namespace algorithm
	{
		/** @addtogroup m53
			@{
		*/

		typedef enum {DEPARTURE_FIRST = 0, ARRIVAL_FIRST = 1} PlanningOrder;

		/** Service determination method.
			- DEPARTURE_TO_ARRIVAL = the service is chosen from a presence time before a departure.
				The arrival will be chosen in the following edges.
			- ARRIVAL_TO_DEPARTURE = the service is chosen from a presence time after an arrival.
				The departure will be chosen in the preceding edges.
		*/
		typedef enum {
			ARRIVAL_TO_DEPARTURE,
			DEPARTURE_TO_ARRIVAL
		} PlanningPhase ;

		/** @} */
	}
}

#endif // SYNTHESE_routeplanner_RoutePlannerTypes_h__
