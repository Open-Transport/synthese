
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
#include "15_env/Types.h"
#include "15_env/VertexAccessMap.h"
#include "15_env/ServiceUse.h"

#include "33_route_planner/BestVertexReachesMap.h"
#include "33_route_planner/JourneyLegComparator.h"

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
			@ingroup m33
		*/
		class RoutePlanner
		{
		public:

		private:
			

			const synthese::env::Place* _origin;  //<! Origin place for route planning.
			const synthese::env::Place* _destination;  //!< Destination place for route planning.

			synthese::env::VertexAccessMap _originVam;
			synthese::env::VertexAccessMap _destinationVam;

			const AccessParameters _accessParameters;
		    
			const PlanningOrder _planningOrder;  //!< Define planning sequence.


			synthese::time::DateTime _minDepartureTime;  //!< Min departure time.
			synthese::time::DateTime _maxArrivalTime;  //!< Max arrival time.

			const synthese::time::DateTime _journeySheetStartTime;  //!< Start time of schedule sheet.
			const synthese::time::DateTime _journeySheetEndTime;    //!< End time of schedule sheet.

			const synthese::time::DateTime _calculationTime;    //!< Time of calculation (initialized to current time)

			int _previousContinuousServiceDuration;  //!< Journey duration in previously found continuous service.

			synthese::time::DateTime _previousContinuousServiceLastDeparture;  //!< End time of validity range of previously found continuous service.
		    
			BestVertexReachesMap _bestDepartureVertexReachesMap;  //!< 
			BestVertexReachesMap _bestArrivalVertexReachesMap;  //!< 

			JourneyLegComparator _journeyLegComparatorForBestArrival;
			JourneyLegComparator _journeyLegComparatorForBestDeparture;
		    
	    

		 public:
			 RoutePlanner (const synthese::env::Place* origin,
				 const synthese::env::Place* destination,
				 const AccessParameters& accessParameters,
				 const PlanningOrder& planningOrder,
				 const synthese::time::DateTime& journeySheetStartTime,
				 const synthese::time::DateTime& journeySheetEndTime);

			~RoutePlanner ();


			//! @name Getters/Setters
			//@{

			const synthese::env::Place* getOrigin () const;
			const synthese::env::Place* getDestination () const;

			//@}

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
			void findBestJourney(
				env::Journey& result
				, const env::VertexAccessMap& ovam
				, const env::VertexAccessMap& dvam
				, const AccessDirection& accessDirection
				, const env::Journey& currentJourney
				, bool strictTime
				, bool optim
			);
			
			
			void computeRoutePlanningDepartureArrival(
				env::Journey& result
				, const env::VertexAccessMap& ovam
				, const env::VertexAccessMap& dvam
			);

			env::Journeys computeJourneySheetDepartureArrival ();

			//@}



			//! @name Update methods
			//@{
		    
			//@}



		};
	}
}
#endif
