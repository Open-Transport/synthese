
/** RoutePlanningIntermediateJourney class header.
	@file RoutePlanningIntermediateJourney.hpp

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

#ifndef SYNTHESE_algorithm_RoutePlanningIntermediateJourney_hpp__
#define SYNTHESE_algorithm_RoutePlanningIntermediateJourney_hpp__

#include "Journey.h"
#include "AlgorithmTypes.h"

namespace synthese
{
	namespace algorithm
	{
		class JourneyTemplates;

		//////////////////////////////////////////////////////////////////////////
		/// RoutePlanningIntermediateJourney class.
		///	@ingroup m33
		/// @author Hugues Romain
		/// @date 2010
		class RoutePlanningIntermediateJourney:
			public graph::Journey
		{
		public:
			typedef unsigned int Score;

			class Comparator
			{
			public:
				bool operator()(
					boost::shared_ptr<const RoutePlanningIntermediateJourney> j1,
					boost::shared_ptr<const RoutePlanningIntermediateJourney> j2
				) const	{
					return *j1 > *j2;
				}
			};

		private:
			//! @name Temporary data
			//@{
				boost::posix_time::time_duration	_startApproachDuration;
				boost::posix_time::time_duration	_endApproachDuration;
			//@}

			//! @name Route planning data
			//@{
				PlanningPhase		_phase;
				bool						_endReached;
				boost::optional<graph::Journey::Distance>	_distanceToEnd;
				Score						_score;
				boost::logic::tribool		_similarity;
			//@}

				static const graph::Edge* GetEndEdge(PlanningPhase phase, const graph::ServicePointer& serviceUse);

		public:
			//! @name Constructors
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Empty journey constructor.
				/// @param phase current phase of planning
				RoutePlanningIntermediateJourney(
					PlanningPhase phase
				);



				//////////////////////////////////////////////////////////////////////////
				/// Copy journey constructor.
				/// @param phase current phase of planning
				RoutePlanningIntermediateJourney(
					RoutePlanningIntermediateJourney& model,
					PlanningPhase phase
				);


				//////////////////////////////////////////////////////////////////////////
				/// Builds a journey by adding a service use after an existing journey.
				/// @param journey journey to fill
				/// @param serviceUse leg to add to the journey
				/// @param endIsReached the leg gives access to the destination
				/// @param destinationVam destination vertices
				RoutePlanningIntermediateJourney(
					const RoutePlanningIntermediateJourney& journey,
					const graph::ServicePointer& serviceUse,
					bool endIsReached,
					const graph::VertexAccessMap& destinationVam,
					graph::Journey::Distance distanceToEnd,
					bool similarity,
					Score score
				);



				//////////////////////////////////////////////////////////////////////////
				/// Builds a journey by concatenation of two journeys.
				/// @param journey1 first journey
				/// @param journey2 second journey
				/// @pre The two journeys were created at the same phase.
				RoutePlanningIntermediateJourney(
					const RoutePlanningIntermediateJourney& journey1,
					const RoutePlanningIntermediateJourney& journey2
				);
			//@}

			//! @name Getters
			//@{
				const boost::posix_time::time_duration& getStartApproachDuration() const { return _startApproachDuration; }
				const boost::posix_time::time_duration& getEndApproachDuration() const { return _endApproachDuration; }
				bool getEndReached() const { return _endReached; }
				boost::optional<graph::Journey::Distance> getDistanceToEnd() const { return _distanceToEnd; }
				Score getScore() const { return _score; }
				bool getSimilarity() const { return _similarity; }
			//@}

			//! @name Setters
			//@{
				void setStartApproachDuration(const boost::posix_time::time_duration& value){ _startApproachDuration = value; }
				void setEndApproachDuration(const boost::posix_time::time_duration& value){ _endApproachDuration = value; }
				void setScore(Score value){ _score = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Gets the last reached edge.
			/// @pre The journey must not be empty
			const graph::Edge& getEndEdge() const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the time at the last reached point.
			/// @param includeApproach add the end approach duration to the time
			/// @pre The journey must not be empty
			const boost::posix_time::ptime getEndTime(
				bool includeApproach = true
			) const;

			boost::posix_time::ptime getBeginTime(bool includeApproach = true) const;

			boost::posix_time::ptime getFirstDepartureTime (bool includeApproach = true) const;
			boost::posix_time::ptime getLastDepartureTime (bool includeApproach = true) const;
			boost::posix_time::ptime getFirstArrivalTime (bool includeApproach = true) const;
			boost::posix_time::ptime getLastArrivalTime (bool includeApproach = true) const;

			boost::posix_time::time_duration getDuration (bool includeApproach = true) const;

			const graph::ServicePointer& getEndServiceUse() const;


			//////////////////////////////////////////////////////////////////////////
			/// Comparison between journeys.
			///	@param other Journey to compare with
			///	@return true if the current journey is a best choice than the other one
			/// @author Hugues Romain
			//////////////////////////////////////////////////////////////////////////
			/// The comparison is done in two different ways :
			///	<ul>
			///		<li>If the two journeys start and end at the same place or call
			///		both the origin and destination vams, then the best journey is chosen
			///		according to the comfort criteria</li>
			///		<li>Else, the choice corresponds to the best bet to reach the
			///		destination quickly</li>
			///	</ul>
			bool operator>(const RoutePlanningIntermediateJourney& other) const;

			virtual void clear();
		};
	}
}

#endif // SYNTHESE_algorithm_RoutePlanningIntermediateJourney_hpp__
