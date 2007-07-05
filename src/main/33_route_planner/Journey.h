
/** Journey class header.
	@file Journey.h

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

#ifndef SYNTHESE_ROUTEPLANNER_JOURNEY_H
#define SYNTHESE_ROUTEPLANNER_JOURNEY_H

#include "33_route_planner/Types.h"

#include "15_env/Types.h"

namespace synthese
{
	namespace time
	{
		class DateTime;
	}
	namespace env
	{
		class Edge;
		class Axis;
	}

	namespace routeplanner
	{
		class JourneyLeg;

		/** Journey class.
			@ingroup m33
		*/
		class Journey
		{
		private:

			JourneyLegs	_journeyLegs;
			int			_continuousServiceRange;
			int			_effectiveDuration;
			int			_transportConnectionCount;
			int			_distance;

		 public:

			Journey ();
			~Journey ();


			//! @name Getters
			//@{
				/** Journey legs getter.
					@return const JourneyLegs& The journey legs
					@author Hugues Romain
					@date 2007					
				*/
				const JourneyLegs& getJourneyLegs() const;


				/** Returns the effective amount of time spent
					travelling, excluding tranfer delays.
				*/
				int getEffectiveDuration () const;
	
				int getTransportConnectionCount ();

				/** Continuous service range of this journey.
					@return Range duration in minutes, or 0 if unique service.
				*/
				int getContinuousServiceRange () const;

				int getDistance () const;

			//@}

			//! @name Query methods
			//@{
				bool	empty()	const;
				int getJourneyLegCount () const;
				
				const env::ServiceUse& getJourneyLeg (int index) const;
				const env::ServiceUse& getFirstJourneyLeg () const;
				const env::ServiceUse& getLastJourneyLeg () const;

				const synthese::env::Edge* getOrigin() const;
				const synthese::env::Edge* getDestination() const;

				const synthese::time::DateTime& getDepartureTime () const;
				const synthese::time::DateTime& getArrivalTime () const;

				int getDuration () const;
		
		
				/** Detects max alarm level in journey.
			      
					For each journey leg, 4 cases are possible :
				- Alert on origin
				- Service with reservation rule
				- Service alert
				- Alert on destination
			      
				@return Maximum journey alert level.
				*/
				int getMaxAlarmLevel () const;


				void setContinuousServiceRange (int continuousServiceRange);

				/** Comparison between journeys doing the same relation..
					@param other Journey to compare with
					@param direction Direction of the route planning
					@return true if the current journey is a best choice than the other one
				*/
				bool isBestThan(const Journey& other, const AccessDirection& direction) const;


				bool verifyAxisConstraints(const env::Axis* axis) const;
			//@}



			//! @name Update methods
			//@{
				void clear ();

				void prepend (const env::ServiceUse& leg);
				void prepend (const Journey& journey);

				void append (const env::ServiceUse& leg);
				void append (const Journey& journey);
			//@}

			Journey& operator = (const Journey& ref);


		};


	}
}

#endif
