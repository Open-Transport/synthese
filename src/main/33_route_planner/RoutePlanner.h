
/** RoutePlanner class header.
	@file RoutePlanner.h

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

#ifndef SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H
#define SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H

#include <deque>
#include <vector>
#include <map>

#include "04_time/DateTime.h"

#include "15_env/Place.h"
#include "15_env/VertexAccessMap.h"
#include "15_env/ServiceUse.h"

#include "33_route_planner/Types.h"

namespace synthese
{

	namespace time
	{
		class DateTime;
	}

	namespace env
	{
		class Edge;
		class Path;
		class Vertex;
		class Service;
		class SquareDistance;
		class ServicePointer;
		class Journey;
	}

	namespace routeplanner
	{
		typedef enum {DEPARTURE_FIRST, ARRIVAL_FIRST} PlanningOrder;

		/** Route planner class.
			@ingroup m53
		*/
		class RoutePlanner
		{
		public:
			class Result
			{
			public:
				JourneyBoardJourneys journeys;
				void clear();
				~Result();
			};

		private:
			
			//! @name Parameters
			//@{
				env::VertexAccessMap _originVam;
				env::VertexAccessMap _destinationVam;
				const AccessParameters _accessParameters;
				const time::DateTime _journeySheetStartTime;  //!< Start time of schedule sheet.
				const time::DateTime _journeySheetEndTime;    //!< End time of schedule sheet.
				const PlanningOrder _planningOrder;  //!< Define planning sequence.
				const time::DateTime _calculationTime;    //!< Time of calculation (initialized to current time)
				const int _maxSolutionsNumber;
			//@}

			//! @name Working variables
			//@{
				time::DateTime _minDepartureTime;  //!< Min departure time.
				time::DateTime _maxArrivalTime;  //!< Max arrival time.
				int _previousContinuousServiceDuration;  //!< Journey duration in previously found continuous service.
				time::DateTime _previousContinuousServiceLastDeparture;  //!< End time of validity range of previously found continuous service.
				Result			_result;
				 //!< 
			//@}

		public:
			/** Constructor.
				@param origin Origin place
				@param destination Destination place
				@param accessParameters Access Parameters
				@param planningOrder Planning order
				@param journeySheetStartTime Start date time
				@param journeySheetEndTime End date time
				@param maxSolutionsNumber Maximum of solutions number
				@author Hugues Romain
				@date 2007
			*/
			RoutePlanner(
				 const env::Place* origin,
				 const env::Place* destination,
				 const AccessParameters& accessParameters,
				 const PlanningOrder& planningOrder,
				 const time::DateTime& journeySheetStartTime,
				 const time::DateTime& journeySheetEndTime
				 , int maxSolutionsNumber = UNKNOWN_VALUE
			);

			~RoutePlanner ();


			//! @name Query methods
			//@{



			/** Best journey finder.
				@param ovam Vertex access map containing each departure physical stops.
				@param dvam Vertex access map containing each destination stops.
				@param accessDirection Type of computing : search of better arrival or of a better departure
				@param currentJourney Journey conducting to the departure vam
				@param sctrictTime Filter : 
					- true : solutions starting at the vam time are only selected
					- false : all solutions are selected
				@param optim Filter : 
					- true : solutions allowing a comfort raising and a time saving are selected
					- false :solutions allowing a time saving are only selected
			*/
/*			void findBestJourney(
				env::Journey& result
				, const env::VertexAccessMap& ovam
				, const env::VertexAccessMap& dvam
				, const env::Journey& currentJourney
				, bool strictTime
			);
*/			

			void findBestJourney(
				env::Journey& result
				, const env::VertexAccessMap& startVam
				, const env::VertexAccessMap& endVam
				, const time::DateTime& startTime
				, bool strictTime
			);

			void computeRoutePlanningDepartureArrival(
				env::Journey& result
				, const env::VertexAccessMap& ovam
				, const env::VertexAccessMap& dvam
			);

			/** Launch of the route planning, applying the "from the departure to the arrival" method.
				@return JourneyBoardJourneys The founded journeys
				@warning The journeys must be deleted after use to avoid memory leak
				@author Hugues Romain
				@date 2007
			*/
			const Result& computeJourneySheetDepartureArrival ();

			//@}
		};
	}
}
#endif
