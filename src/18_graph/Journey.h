
/** Journey class header.
	@file Journey.h

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

			//! @name Internal data
			//@{
				boost::posix_time::time_duration	_effectiveDuration;
				std::size_t _transportConnectionCount;
				double			_distance;

				void _updateInternalData(
					const ServicePointer& leg
				);
			//@}
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Empty journey constructor.
			Journey();



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
				boost::posix_time::time_duration getEffectiveDuration () const { return _effectiveDuration; }



				double getDistance () const { return _distance; }
				std::size_t getTransportConnectionsCount() const { return _transportConnectionCount; }
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



				/** Continuous service range of this journey.
					@return Range duration in minutes, or 0 if unique service.
				*/
				boost::posix_time::time_duration getContinuousServiceRange() const;



				/** Detects max alarm level in journey.

					For each journey leg, 4 cases are possible :
				- Alert on origin
				- Service with reservation rule
				- Service alert
				- Alert on destination

				@return Maximum journey alert level.
				*/
				int getMaxAlarmLevel () const;

				boost::logic::tribool getReservationCompliance(
					bool ignoreReservationDeadline,
					UseRule::ReservationDelayType reservationRulesDelayType = UseRule::RESERVATION_INTERNAL_DELAY
				) const;
				boost::posix_time::ptime getReservationDeadLine(
					UseRule::ReservationDelayType reservationRulesDelayType = UseRule::RESERVATION_INTERNAL_DELAY
				) const;
			//@}



			//! @name Update methods
			//@{
				void prepend(const ServicePointer& leg);
				void append(const ServicePointer& leg);
				void prepend (const Journey& journey);
				void append (const Journey& journey);

				virtual void clear();

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
