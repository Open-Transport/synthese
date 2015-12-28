
/** PublicBikeJourneyPlanner class header.
	@file PublicBikeJourneyPlanner.h

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

#ifndef SYNTHESE_publicbiking_PublicBikeJourneyPlanner_h__
#define SYNTHESE_publicbiking_PublicBikeJourneyPlanner_h__

#include "TimeSlotRoutePlanner.h"
#include "PublicBikeJourneyPlannerResult.hpp"
#include "AlgorithmTypes.h"
#include "AccessParameters.h"
#include "GraphTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace geography
	{
		class Place;
	}
	namespace public_biking
	{
		/** Public Bike journey planner.
			@ingroup m65
		*/
		class PublicBikeJourneyPlanner:
			public algorithm::TimeSlotRoutePlanner
		{
		private:
			const geography::Place* const _departurePlace;
			const geography::Place* const _arrivalPlace;
			const graph::AccessParameters _journeyParameters;


		public:
			PublicBikeJourneyPlanner(
				const geography::Place* origin,
				const geography::Place* destination,
				const boost::posix_time::ptime& lowerDepartureTime,
				const boost::posix_time::ptime& higherDepartureTime,
				const boost::posix_time::ptime& lowerArrivalTime,
				const boost::posix_time::ptime& higherArrivalTime,
				const boost::optional<std::size_t>	maxSolutionsNumber,
				const graph::AccessParameters		accessParameters,
				const graph::AccessParameters		journeyParameters,
				const algorithm::PlanningOrder		planningOrder,
				const algorithm::AlgorithmLogger& logger
			);

			PublicBikeJourneyPlannerResult run() const;
		};
}	}

#endif // SYNTHESE_ptrouteplanner_RoadRoutePlanner_h__
