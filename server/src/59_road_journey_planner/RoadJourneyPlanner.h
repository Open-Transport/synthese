
/** RoadJourneyPlanner class header.
	@file RoadJourneyPlanner.h

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

#ifndef SYNTHESE_ptrouteplanner_RoadRoutePlanner_h__
#define SYNTHESE_ptrouteplanner_RoadRoutePlanner_h__

#include "TimeSlotRoutePlanner.h"
#include "RoadJourneyPlannerResult.h"
#include "AlgorithmTypes.h"
#include "AccessParameters.h"
#include "GraphTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace algorithm
	{
		class AlgorithmLogger;
	}

	namespace geography
	{
		class Place;
	}

	namespace road_journey_planner
	{
		/** Road journey planner.
			@ingroup m59
		*/
		class RoadJourneyPlanner:
			public algorithm::TimeSlotRoutePlanner
		{
		private:
			const geography::Place* const _departurePlace;
			const geography::Place* const _arrivalPlace;
			const bool _allowSmallPlanning;


		public:
			RoadJourneyPlanner(
				const geography::Place* origin,
				const geography::Place* destination,
				const boost::posix_time::ptime& lowerDepartureTime,
				const boost::posix_time::ptime& higherDepartureTime,
				const boost::posix_time::ptime& lowerArrivalTime,
				const boost::posix_time::ptime& higherArrivalTime,
				const boost::optional<std::size_t>	maxSolutionsNumber,
				const graph::AccessParameters		accessParameters,
				const algorithm::PlanningOrder		planningOrder,
				const algorithm::AlgorithmLogger& logger,
				const bool						allowSmallPlanning = false
			);

			RoadJourneyPlannerResult run() const;
		};
}	}

#endif // SYNTHESE_ptrouteplanner_RoadRoutePlanner_h__
