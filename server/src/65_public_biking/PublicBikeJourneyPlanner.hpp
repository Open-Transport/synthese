
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

#include "AlgorithmTypes.h"
#include "AccessParameters.h"
#include "AStarShortestPathCalculator.hpp"
#include "GraphTypes.h"
#include "PublicBikeJourneyPlannerResult.hpp"

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>

namespace synthese
{
	namespace geography
	{
		class Place;
	}
	namespace public_biking
	{
		class PublicBikeStation;

		/** Public Bike journey planner.
			@ingroup m65
		*/
		class PublicBikeJourneyPlanner
		{
		private:
			const geography::Place* const _departurePlace;
			const geography::Place* const _arrivalPlace;
			const graph::AccessParameters _journeyParameters;
			const boost::posix_time::ptime& _departureTime;
			const boost::posix_time::ptime& _higherArrivalTime;
			const graph::AccessParameters _accessParameters;
			const algorithm::AlgorithmLogger& _logger;

			graph::ServicePointer _constructStartJourney(
				const PublicBikeStation* origin,
				const graph::ServicePointer firstSP,
				graph::VertexAccess va
			) const;
			graph::ServicePointer _constructEndJourney(
				const graph::Vertex* lastVertex,
				const PublicBikeStation* destination,
				const boost::posix_time::time_duration& timeShiftCrossing,
				graph::VertexAccess va
			) const;
			graph::ServicePointer _constructAStarStartJourney(
				const PublicBikeStation* origin,
				const road::RoadChunkEdge* firstRoadChunkEdge,
				const boost::posix_time::time_duration& timeToReachBikeStation,
				const graph::VertexAccess va
			) const;


		public:
			PublicBikeJourneyPlanner(
				const geography::Place* origin,
				const geography::Place* destination,
				const boost::posix_time::ptime& departureTime,
				const boost::posix_time::ptime& higherArrivalTime,
				const graph::AccessParameters		accessParameters,
				const graph::AccessParameters		journeyParameters,
				const algorithm::AlgorithmLogger& logger
			);

			PublicBikeJourneyPlannerResult run() const;

			typedef boost::tuple<
				graph::Journey, // Approach journey at the departure (walk to bike station)
				algorithm::AStarShortestPathCalculator::ResultPath, //A* result (bike)
				graph::Journey // Approach journey at the arrival (walk from bike station)
			> PublicBikeJourneyPlannerAStarResult;
			std::vector<PublicBikeJourneyPlannerAStarResult> runAStar() const;
		};
}	}

#endif // SYNTHESE_ptrouteplanner_RoadRoutePlanner_h__
