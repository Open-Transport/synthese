
/** TimeSlotRoutePlanner class header.
	@file TimeSlotRoutePlanner.h

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

#ifndef SYNTHESE_routeplanner_TimeSlotRoutePlanner_h__
#define SYNTHESE_routeplanner_TimeSlotRoutePlanner_h__

#include "RoutePlannerTypes.h"
#include "Journey.h"
#include "AccessParameters.h"
#include "Log.h"
#include "VertexAccessMap.h"

#include <deque>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace algorithm
	{
		class RoutePlanner;

		/** TimeSlotRoutePlanner class.
			@ingroup m53
		*/
		class TimeSlotRoutePlanner
		{
		public:
			typedef std::deque<graph::Journey> Result;

		protected:
			//! @name Parameters
			//@{
				const graph::VertexAccessMap				_originVam;
				const graph::VertexAccessMap				_destinationVam;
				const time::DateTime	_lowestDepartureTime;  //!< Start time of schedule sheet.
				const time::DateTime	_highestDepartureTime;    //!< End time of schedule sheet.
				const time::DateTime	_lowestArrivalTime;  //!< Start time of schedule sheet.
				const time::DateTime	_highestArrivalTime;    //!< End time of schedule sheet.
				const boost::optional<boost::posix_time::time_duration> _maxDuration;
				const boost::optional<std::size_t>	_maxSolutionsNumber;
				const graph::AccessParameters		_accessParameters;
				const PlanningOrder					_planningOrder;  //!< Define planning sequence.
				const graph::GraphIdType			_whatToSearch;
				const graph::GraphIdType			_graphToUse;
				const graph::Journey				_parentContinuousService;
			//@}

			//! @name Logging
			//@{
				std::ostream* const					_logStream;
				const util::Log::Level				_logLevel;
			//@}

			Result _MergeSubResultAndParentContinuousService(
				const Result::value_type& parentContinuousService,
				const Result& subResult
			);
		public:
			/** Constructor for specified time slot route planning.
			*/
			TimeSlotRoutePlanner(
				const graph::VertexAccessMap& originVam,
				const graph::VertexAccessMap& destinationVam,
				const time::DateTime& lowestDepartureTime,
				const time::DateTime& highestDepartureTime,
				const time::DateTime& lowestArrivalTime,
				const time::DateTime& highestArrivalTime,
				const graph::GraphIdType			whatToSearch,
				const graph::GraphIdType			graphToUse,
				boost::optional<boost::posix_time::time_duration> maxDuration,
				boost::optional<std::size_t>	maxSolutionsNumber,
				graph::AccessParameters		accessParameters,
				PlanningOrder					planningOrder,
				std::ostream* logStream = NULL,
				util::Log::Level logLevel = util::Log::LEVEL_NONE
			);

			
			/** Constructor for attempting to optimize a continuous service slot.
				@param originVam
				@param destinationVam
				@param continuousService
				@param maxSolutionsNumber
				@param accessParameters
				@param planningOrder
				@return 
				@author Hugues Romain
				@date 2009				
			*/
			TimeSlotRoutePlanner(
				const graph::VertexAccessMap& originVam,
				const graph::VertexAccessMap& destinationVam,
				const Result::value_type& continuousService,
				const graph::GraphIdType			whatToSearch,
				const graph::GraphIdType			graphToUse,
				boost::optional<boost::posix_time::time_duration> maxDuration,
				boost::optional<std::size_t>	maxSolutionsNumber,
				graph::AccessParameters		accessParameters,
				PlanningOrder					planningOrder,
				std::ostream* logStream = NULL,
				util::Log::Level logLevel = util::Log::LEVEL_NONE
			);

			//! @name Getters
			//@{
				const time::DateTime&		getLowestDepartureTime() const;
				const time::DateTime&		getHighestDepartureTime() const;
				const time::DateTime&		getLowestArrivalTime() const;
				const time::DateTime&		getHighestArrivalTime() const;
			//@}

			/** Launch of the route planning
				@return the result
				@author Hugues Romain
				@date 2007
			*/
			Result run();
		};
	}
}

#endif // SYNTHESE_routeplanner_TimeSlotRoutePlanner_h__
