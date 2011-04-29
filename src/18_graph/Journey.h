
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

#include <deque>
#include <boost/logic/tribool.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include "ServicePointer.h"
#include "GraphTypes.h"

namespace synthese
{
	namespace graph
	{
		class Edge;
		class VertexAccessMap;

		//////////////////////////////////////////////////////////////////////////
		/// Journey using all kind of networks.
		///	@ingroup m35
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// A journey consists in a succession of service uses.
		/// It can represent a continuous service.
		class Journey
		{
		public:
			typedef std::deque<ServicePointer>	ServiceUses;
			typedef unsigned int Distance;

		protected:

			//! @name Content
			//@{
				ServiceUses							_journeyLegs;
			//@}

			//! @name Query cache
			//@{
				boost::posix_time::time_duration	_effectiveDuration;
				int				_transportConnectionCount;
				double			_distance;
			//@}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Empty journey constructor.
			Journey();


			//////////////////////////////////////////////////////////////////////////
			/// Journey constructor from a service pointer.
			Journey(
				const ServicePointer& servicePointer
			);



			//////////////////////////////////////////////////////////////////////////
			/// Builds a journey by adding a service use to an existing journey.
			/// @param journey the journey to fill
			/// @param serviceUse the leg to add to the journey
			/// @param order if true the leg is pushed to the end of the journey, else to the beginning
			Journey(
				const Journey& journey,
				const ServicePointer& serviceUse,
				bool order = true
			);

			//////////////////////////////////////////////////////////////////////////
			/// Builds a journey by concatenation of two journeys.
			/// @param journey1 first journey
			/// @param journey2 second journey
			/// @param order if true the second journey is pushed after the first, else the first is pushed after the second
			Journey(
				const Journey& journey1,
				const Journey& journey2,
				bool order = true
			);

			//////////////////////////////////////////////////////////////////////////
			/// Destructor.
			~Journey();


			//! @name Getters
			//@{
				/** Journey legs getter.
					@return const JourneyLegs& The journey legs
					@author Hugues Romain
					@date 2007
				*/
				const ServiceUses& getServiceUses() const { return _journeyLegs; }
				ServiceUses& getServiceUses() { return _journeyLegs; }


				/** Returns the effective amount of time spent
					travelling, excluding tranfer delays.
				*/
				boost::posix_time::time_duration getEffectiveDuration () const;

				/** Continuous service range of this journey.
					@return Range duration in minutes, or 0 if unique service.
				*/
				boost::posix_time::time_duration			getContinuousServiceRange () const;
			//@}

			//! @name Setters
			//@{
			//@}


			//! @name Query methods
			//@{
				bool empty()	const;
				std::size_t size() const;

				const ServicePointer& getJourneyLeg (std::size_t index) const;
				const ServicePointer& getFirstJourneyLeg () const;
				const ServicePointer& getLastJourneyLeg () const;

				const boost::posix_time::ptime& getFirstDepartureTime () const;
				boost::posix_time::ptime getLastDepartureTime () const;
				const boost::posix_time::ptime& getFirstArrivalTime () const;
				boost::posix_time::ptime getLastArrivalTime () const;

				boost::posix_time::time_duration getDuration () const;

				const Edge* getOrigin() const;
				const Edge* getDestination() const;

				double getDistance () const;


				/** Detects max alarm level in journey.

					For each journey leg, 4 cases are possible :
				- Alert on origin
				- Service with reservation rule
				- Service alert
				- Alert on destination

				@return Maximum journey alert level.
				*/
				int getMaxAlarmLevel () const;

				boost::logic::tribool	getReservationCompliance(bool ignoreReservationDeadline) const;
				boost::posix_time::ptime			getReservationDeadLine() const;
			//@}



			//! @name Update methods
			//@{
				void prepend(const ServicePointer& leg);
				void append(const ServicePointer& leg);
				void prepend (const Journey& journey);
				void append (const Journey& journey);

				void clear ();

				void shift(
					boost::posix_time::time_duration duration
				);



				//////////////////////////////////////////////////////////////////////////
				/// Sets the continuous service range of each leg at the specified value.
				/// @param duration the continuous service range to set
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				void forceContinuousServiceRange(
					boost::posix_time::time_duration duration
				);
			//@}
		};
	}
}

#endif
