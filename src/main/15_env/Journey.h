
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

		/** Journey class.
			@ingroup m15
		*/
		class Journey
		{
		private:
			//! @name Content
			//@{
				JourneyLegs		_journeyLegs;
			//@}

			//! @name Supplemental data
			//@{
				AccessDirection	_method;
				int				_continuousServiceRange;
			//@}

			//! @name Query cache
			//@{
				int				_effectiveDuration;
				int				_transportConnectionCount;
				int				_distance;
			//@}
			
			//! @name Oriented supplemental data
			//@{
				int				_startApproachDuration;
				int				_endApproachDuration;
			//@}

		 public:
			Journey(AccessDirection method = TO_DESTINATION);
			~Journey ();


			//! @name Getters
			//@{
				/** Journey legs getter.
					@return const JourneyLegs& The journey legs
					@author Hugues Romain
					@date 2007					
				*/
				const JourneyLegs& getJourneyLegs() const;

				AccessDirection getMethod() const;

				/** Returns the effective amount of time spent
					travelling, excluding tranfer delays.
				*/
				int getEffectiveDuration () const;
	
				int getTransportConnectionCount ();

				/** Continuous service range of this journey.
					@return Range duration in minutes, or 0 if unique service.
				*/
				int getContinuousServiceRange () const;
			//@}

			//! @name Setters
			//@{
				void setContinuousServiceRange (int continuousServiceRange);
			//@}

			//! @name Orientation relative methods
			//@{
				const env::ServiceUse& getEndServiceUse() const;
				const env::ServiceUse& getStartServiceUse() const;
				void setEndApproachDuration(int duration);
				void setStartApproachDuration(int duration);
			//@}

			//! @name Query methods
			//@{
				bool empty()	const;
				int getJourneyLegCount () const;
				
				const env::ServiceUse& getJourneyLeg (int index) const;
				const env::ServiceUse& getFirstJourneyLeg () const;
				const env::ServiceUse& getLastJourneyLeg () const;
				
				const env::Edge* getOrigin() const;
				const env::Edge* getDestination() const;

				time::DateTime getDepartureTime () const;
				time::DateTime getArrivalTime () const;

				int getDuration () const;
				int getDistance () const;
		
		
				/** Detects max alarm level in journey.
			      
					For each journey leg, 4 cases are possible :
				- Alert on origin
				- Service with reservation rule
				- Service alert
				- Alert on destination
			      
				@return Maximum journey alert level.
				*/
				int getMaxAlarmLevel () const;

				/** Comparison between journeys doing the same relation..
					@param other Journey to compare with
					@return true if the current journey is a best choice than the other one
					@warning Journey objects must have same method
				*/
				bool isBestThan(const Journey& other) const;


				bool verifyAxisConstraints(const env::Axis* axis) const;
			//@}



			//! @name Update methods
			//@{
				void clear ();

				void push(const ServiceUse& leg);
				void push(const Journey& journey);
				
				void prepend (const ServiceUse& leg);
				void prepend (const Journey& journey);

				void append (const env::ServiceUse& leg);
				void append (const Journey& journey);

				void shift(int duration, int continuousServiceRange = UNKNOWN_VALUE);
				void reverse();
			//@}

			Journey& operator = (const Journey& ref);


		};


	}
}

#endif
