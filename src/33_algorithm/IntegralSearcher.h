
/** IntegralSearcher class header.
	@file IntegralSearcher.h

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

#ifndef SYNTHESE_routeplanner_IntegralSearcher_h__
#define SYNTHESE_routeplanner_IntegralSearcher_h__

#include "GraphTypes.h"
#include "AccessParameters.h"
#include "GraphModuleTemplate.h"
#include "RoutePlanningIntermediateJourney.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace graph
	{
		class Vertex;
		class VertexAccessMap;
		class Edge;
		class Hub;
	}

	namespace algorithm
	{
		class AlgorithmLogger;
		class BestVertexReachesMap;
		class JourneyTemplates;
		class JourneysResult;
		class RoutePlanningIntermediateJourney;

		/** IntegralSearcher class.
			@ingroup m33

			This class provides an algorithm which returns all the vertices available in an area.

			The area is defined by several constraints.

			This algorithm can be used in a route planning : it handles optionally the concept of searched place (destination).
		*/
		class IntegralSearcher
		{
		private:
			typedef graph::Edge* (graph::Edge::*PtrEdgeStep) () const;

			struct _JourneyUsefulness
			{
				bool canBeAResultPart;
				bool continueToTraverseThePath;
				_JourneyUsefulness(bool _canBeAResultPart, bool _continueToTraverseThePath);
			};


			//! @name Parameters
			//@{
				const graph::AccessParameters				_accessParameters;
				const PlanningPhase							_accessDirection;
				const graph::GraphIdType					_whatToSearch;
				const bool									_searchOnlyNodes;
				const graph::GraphIdType					_graphToUse;
				JourneysResult&								_result;
				BestVertexReachesMap&						_bestVertexReachesMap;
				const boost::posix_time::ptime&				_originDateTime;
				const boost::posix_time::ptime&				_minMaxDateTimeAtOrigin;
				boost::posix_time::ptime&					_minMaxDateTimeAtDestination;
				const bool									_inverted;	//!< Indicates that the AccessDirection is the contrary to the planning order (2nd phase)
				const bool									_optim;
				const AlgorithmLogger&						_logger;
				boost::optional<boost::posix_time::time_duration>	_maxDuration;
				const double								_vmax;
				bool										_ignoreReservation;
				bool										_enableTheoretical;
				bool										_enableRealTime;
			//@}

			//! @name Route planning data
			//@{
				const graph::VertexAccessMap&				_destinationVam;	//!< Can be a departure or an arrival, according to _accesDirection
				const int									_totalDistance;
				boost::optional<const JourneyTemplates&>	_journeyTemplates;	//!< For similarity test
			//@}


			void _integralSearch(
				const graph::VertexAccessMap& vertices,
				const RoutePlanningIntermediateJourney& startJourney,
				const boost::posix_time::ptime& originDateTime,
				const boost::posix_time::ptime& minMaxOriginDateTime,
				boost::optional<std::size_t> maxDepth,
				boost::optional<boost::posix_time::time_duration> totalDuration
			);


			//////////////////////////////////////////////////////////////////////////
			/// Sets the informations about the position of the journey as a route
			/// planning result.
			/// @param endIsReached indicates that the journey has reached the goal of
			///		a route planning.
			/// @param goal the goal vertex access map
			/// @param bestTimeAtGoal the best time found for an other journey to reach
			///		the goal
			/// Sets :
			///		- route planning informations
			///		- end arrival time
			void _setJourneyRoutePlanningInformations(
				graph::Journey& journey,
				bool endIsReached,
				boost::optional<boost::posix_time::time_duration> totalDuration
			) const;

		public:

			//////////////////////////////////////////////////////////////////////////
			/// Computes and stores the score of a journey.
			/// @param totalDuration the duration of the best journey found by the route planner (undefined if no journey has been found at the time)
			/// @param journeyTemplates journeys to compare with for similarity test
			/// @param originDateTime time at the beginning of the search
			/// @param totalDistance distance between origin and destination places
			/// @author Hugues Romain
			/// @date 2009-2010
			//////////////////////////////////////////////////////////////////////////
			/// The score is between 0 and 1000.
			/// 0 is a special value indicating that the journey
			///
			/// @image html scores_noresult.png
			/// @image html scores_result.png
			///
			/// <h3>Attachments</h3>
			/// <ul><li><a href="include/test-score.xslx">Score simulation table</a></li></ul>
			RoutePlanningIntermediateJourney::Score _getScore(
				boost::optional<boost::posix_time::time_duration> totalDuration,
				int distanceToEnd,
				boost::posix_time::time_duration journeyDuration,
				const graph::Hub& hub
			) const;


			IntegralSearcher(PlanningPhase										accessDirection,
				const graph::AccessParameters&						accessParameters,
				graph::GraphIdType									whatToSearch,
				bool												searchOnlyNodes,
				graph::GraphIdType									graphToUse,
				JourneysResult&										result,
				BestVertexReachesMap&								bestVertexReachesMap,
				const graph::VertexAccessMap&						destinationVam,
				const boost::posix_time::ptime&						originDateTime,
				const boost::posix_time::ptime&						minMaxDateTimeAtOrigin,
				boost::posix_time::ptime&							minMaxDateTimeAtDestination,
				bool												inverted,
				bool												optim,
				boost::optional<boost::posix_time::time_duration>	maxDuration,
				double												vmax,
				bool												ignoreReservation,
				const AlgorithmLogger&								logger,
				int													totalDistance = 0,
				boost::optional<const JourneyTemplates&>			journeyTemplates = boost::optional<const JourneyTemplates&>(),
				bool 												enableTheoretical = true,
				bool												enableRealTime = true
			);



			//////////////////////////////////////////////////////////////////////////
			/// Integral search from the start vertex access map.
			/// @param startVam the search is launched at this vertices
			///	@param maxDepth Maximum recursion depth.
			void integralSearch(
				const graph::VertexAccessMap& vertices,
				boost::optional<std::size_t> maxDepth,
				boost::optional<boost::posix_time::time_duration> totalDuration
			);



			//////////////////////////////////////////////////////////////////////////
			/// Integral search after a journey (transfer).
			/// @param startJourney connecting journey
			/// @param maxDepth limit of transfers
			void integralSearch(
				const RoutePlanningIntermediateJourney& startJourney,
				boost::optional<std::size_t> maxDepth,
				boost::optional<boost::posix_time::time_duration> totalDuration,
				boost::optional<boost::posix_time::time_duration> maxTransferWaitingTime
			);



			/** Journey utility evaluation.
				@param journey Journey to evaluate
				@return pair<bool,bool> Possible values : false/false, true/true, false/true
					- first : utility to store the journey as result or a future result part.
					- second : utility to continue to traverse the rest of the path
				@author Hugues Romain
				@date 2007
			*/
			_JourneyUsefulness evaluateJourney(
				const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey,
				bool isGoalReached
			) const;



			//! @name Getters
			//@{
				const boost::posix_time::ptime& getOriginDateTime() const;
			//@}
		};
	}
}

#endif // SYNTHESE_routeplanner_IntegralSearcher_h__
