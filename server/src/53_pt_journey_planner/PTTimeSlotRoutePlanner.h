
/** PTTimeSlotRoutePlanner class header.
	@file PTTimeSlotRoutePlanner.h

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

#ifndef SYNTHESE_ptrouteplanner_PTTimeSlotRoutePlanner_h__
#define SYNTHESE_ptrouteplanner_PTTimeSlotRoutePlanner_h__

#include "TimeSlotRoutePlanner.h"
#include "PTRoutePlannerResult.h"
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

	namespace pt
	{
		class StopArea;
	}

	namespace pt_journey_planner
	{
		//////////////////////////////////////////////////////////////////////////
		/// Public transportation route planner.
		///	@ingroup m53
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Input : 2 places
		/// Output : all solutions inside a time period between the two points,
		/// composed by :
		/// <ul>
		///		<li>road approach between the departure point and a physical stop
		///		(can be empty if the departure point is a physical stop and if the
		///		best solution runs from it)</li>
		///		<li>public transportation journey with or without any transfer</li>
		///		<li>road approach between the end stop and the arrival point (can be
		///		empty if the journey goes directly to the arrival place)</li>
		///	</ul>
		///
		/// When the departure and arrival places are not far away of each other,
		/// it is possible to reach the arrival from the departure, using only the
		/// road network.
		///
		/// If the _showFullRoadJourney attribute is on, the class returns a continuous
		/// service valid the entire day, representing the roads to use and the duration
		/// of each leg. If sometimes the use of public transportation is faster than the
		/// full road journey, then the public transportation solution is returned and the
		/// continuous service is broken at the period where the public transportation
		/// solution is the best one.
		///
		/// If the _showFullRoadJourney attribute is off, then the class returns only the
		/// public transportation solutions.
		///
		/// The _showFullRoadJourney has no effect on the route planning algorithm. It
		/// controls only the output.
		/// RULE-104
		class PTTimeSlotRoutePlanner:
			public algorithm::TimeSlotRoutePlanner
		{
		private:
			const geography::Place* const _departurePlace;
			const geography::Place* const _arrivalPlace;
			const geography::Place* const _departureParking;
			const geography::Place* const _arrivalParking;
			bool _showFullRoadJourney;
			bool _startWithCar;
			bool _endWithCar;


			// this internal class is used to sort the stop areas equipped with relay parks
			// by their distance from a given point (a departure or arrival place)
			class SortableStopArea
			{
			private:
				const pt::StopArea* _stopArea;
				int _distanceToCenter;
				std::string _opCode;

			public:
				SortableStopArea(const pt::StopArea* stopArea, int distanceToCenter);
				bool operator<(SortableStopArea const &otherStopArea) const;
				const pt::StopArea* getStopArea() const;
				int getDistanceToCenter() const;
				std::string getOpCode() const;
			};

			typedef std::set<SortableStopArea> SortableStopAreaSet;


			//////////////////////////////////////////////////////////////////////////
			/// Search of stops reachable from the departure or the
			/// arrival place by free DRT.
			/// Only destination and transfer stops are returned.
			/// @param vam the points to start the search from
			/// @param destinationVam the points to reach if possible
			/// @param direction the search is for departure or arrival places
			/// @return the list of reachable stops with the duration of approach,
			/// containing the departure and arrival places with empty approach, and
			/// other stops with non empty approach, using the road network to reach them.
			/// @author Hugues Romain
			void _extendByFreeDRT(
				graph::VertexAccessMap& vam,
				const graph::VertexAccessMap& destinationVam,
				algorithm::PlanningPhase direction
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Compute journeys using both personal car and public transportation
			/// @author Olivier Vernhet
			PTRoutePlannerResult _computeCarPTJourney() const;

			//////////////////////////////////////////////////////////////////////////
			/// Compute journeys using both personal car and public transportation,
			/// using a user chosen parking
			/// @author Olivier Vernhet
			PTRoutePlannerResult _computeCarPTJourneyWithChosenParking() const;

			//////////////////////////////////////////////////////////////////////////
			/// Compute journeys using both personal car and public transportation,
			/// proposing the best parking to the user
			/// @author Olivier Vernhet
			PTRoutePlannerResult _computeCarPTJourneyWithBestParking() const;

			//////////////////////////////////////////////////////////////////////////
			/// Prints journeys as debug logs
			/// @param journeys the list of journeys to print
			/// @author Olivier Vernhet
			void _printJourneys(const TimeSlotRoutePlanner::Result& journeys) const;

			//////////////////////////////////////////////////////////////////////////
			/// Finds all the stop areas with a relay park around a place and sorts
			/// them by ascending distance
			/// @param origin the place around which relay parks are searched
			/// @param maxDistance the maximum search radius
			/// @param relayParks the set in which the stop areas will be stored
			/// @author Olivier Vernhet
			void _findRelayParks(const geography::Place* origin, const int maxDistance, SortableStopAreaSet& relayParks) const;


		public:
			PTTimeSlotRoutePlanner(
				const geography::Place* origin,
				const geography::Place* destination,
				const boost::posix_time::ptime& lowestDepartureTime,
				const boost::posix_time::ptime& highestDepartureTime,
				const boost::posix_time::ptime& lowestArrivalTime,
				const boost::posix_time::ptime& highestArrivalTime,
				const boost::optional<std::size_t>	maxSolutionsNumber,
				const graph::AccessParameters		accessParameters,
				const algorithm::PlanningOrder		planningOrder,
				bool								ignoreReservation,
				const algorithm::AlgorithmLogger& algorithmLogger,
				boost::optional<boost::posix_time::time_duration> maxTransferDuration = boost::optional<boost::posix_time::time_duration>(),
				boost::optional<double> minMaxDurationRatioFilter = boost::optional<double>(),
				bool enableTheoretical = true,
				bool enableRealTime = true,
				graph::UseRule::ReservationDelayType reservationRulesDelayType = graph::UseRule::RESERVATION_INTERNAL_DELAY
			);

			PTTimeSlotRoutePlanner(
				const geography::Place*				origin,
				const geography::Place*				destination,
				const geography::Place*				originParking,
				const geography::Place*				destinationParking,
				bool								startWithCar,
				bool								endWithCar,
				const boost::posix_time::ptime&		lowestDepartureTime,
				const boost::posix_time::ptime&		highestDepartureTime,
				const boost::posix_time::ptime&		lowestArrivalTime,
				const boost::posix_time::ptime&		highestArrivalTime,
				const boost::optional<std::size_t>	maxSolutionsNumber,
				const graph::AccessParameters		accessParameters,
				const algorithm::PlanningOrder		planningOrder,
				bool								ignoreReservation,
				const algorithm::AlgorithmLogger&	algorithmLogger,
				boost::optional<boost::posix_time::time_duration> maxTransferDuration = boost::optional<boost::posix_time::time_duration>(),
				boost::optional<double> minMaxDurationRatioFilter = boost::optional<double>(),
				bool enableTheoretical = true,
				bool enableRealTime = true,
				graph::UseRule::ReservationDelayType reservationRulesDelayType = graph::UseRule::RESERVATION_INTERNAL_DELAY
			);

			PTRoutePlannerResult run() const;
		};
	}
}

#endif // SYNTHESE_ptrouteplanner_PTTimeSlotRoutePlanner_h__
