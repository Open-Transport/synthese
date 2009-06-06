
/** PTUseRule class header.
	@file PTUseRule.h

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

#ifndef SYNTHESE_PTUseRule_h__
#define SYNTHESE_PTUseRule_h__

#include "DateTime.h"
#include "UseRule.h"
#include "Registrable.h"
#include "Registry.h"
#include <string>

namespace synthese
{
	namespace pt
	{
		/** Public transportation use rule class.
			@ingroup m35
		*/
		class PTUseRule
		:	public util::Registrable,
			public graph::UseRule
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<PTUseRule>	Registry;


			//////////////////////////////////////////////////////////////////////////
			/// Reservation rule type.
			///	- FORBIDDEN : it is impossible to book a seat on the service
			///	- COMPULSORY : it is impossible to use the service without having booked a seat
			///	- OPTIONAL : is is possible to book a place on the service, but it is possible to use 
			///   the service without having booked a seat
			///	- MIXED_BY_DEPARTURE_PLACE : it is impossible to use the service without having booked 
			///   a place, except for journeys beginning at several places, defined in the path.
			typedef enum {
				RESERVATION_RULE_FORBIDDEN = 0,
				RESERVATION_RULE_COMPULSORY = 1,
				RESERVATION_RULE_OPTIONAL = 2,
				RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE = 3
			} ReservationRuleType;

			static const PTUseRule FORBIDDEN_USE_RULE;


		private:

			std::string _name;

			//! @name Access
			//@{
				////
				/// Maximal person number which can be served
				/// The maximum value of the attribute seems unlimited capacity
				AccessCapacity _accessCapacity;
			//@}
				
			//! @name Reservation
			//@{
				////
				/// Type of the reservation rule.
				ReservationRuleType		_reservationType;
				
				////
				/// Whether reference departure time is the line run departure time at its origin (true)
				/// or client departure time (false).
				bool _originIsReference;

				////
				/// Minimum delay in minutes between reservation and reference moment
				int _minDelayMinutes;
				
				int _minDelayDays;   //!< Minimum delay in days between reservation and reference moment
				int _maxDelayDays;  //!< Maxium number of days between reservation and departure.

				time::Hour _hourDeadLine; //!< Latest reservation hour the last day open for reservation

			//@}
			
			
		public:
			////
			/// Constructor.
			/// The constructor builds a most permissive rule :
			///  - allowed access
			///  - no reservation
			///  - unlimited capacity
			PTUseRule(
				util::RegistryKeyType key = UNKNOWN_VALUE
			);
			
			//! @name Getters
			//@{
				virtual AccessCapacity		getAccessCapacity() const;
				bool				getOriginIsReference()			const;
				const time::Hour&	getHourDeadLine()				const;
				int					getMinDelayDays()				const;
				int					getMinDelayMinutes()			const;
				int					getMaxDelayDays()				const;
				ReservationRuleType	getReservationType()			const;
				const std::string&	getName()						const;
			//@}
			
			//! @name Setters
			//@{
				void setHourDeadLine (const synthese::time::Hour& hourDeadLine);
				void setMinDelayMinutes (int minDelayMinutes);
				void setMinDelayDays (int minDelayDays);
				void setMaxDelayDays (int maxDelayDays);
				void setOriginIsReference (bool originIsReference);
				void setReservationType(ReservationRuleType value);
				void setName(const std::string& value);
				void setAccessCapacity(AccessCapacity value);
			//@}


			/** Reference function for reservation dead line calculation.
				@param originTime Time of start of the corresponding service
				@param departureTime Desired departure time.

				It is done according to the following steps:
					- Choice of reference time (client departure or line run departure at origin)
					- Calculation 1 : x minutes before reference time :
					- Decrease of _minDelayMinutes before reference

					- Calculation 2 : x days before reference time :
					- Decrease of _minDelayDays before reference
					- Sets hour to _hourDeadLine

					- The smallest date time is chosen.

				If no explicit rule defines the reservation dead line, 
				the actual reservation time is returned.
			*/
			virtual time::DateTime getReservationDeadLine (
				const time::DateTime& originTime,
				const time::DateTime& departureTime
			) const;



			/** Reference function for calculation of the date time of the opening of reservations.
				@param reservationTime Time when booking is done.
				@return The minimum date time to make a reservation.
			
				If no explicit rule defines this minimum time, the actual reservation time is returned.
			*/
			virtual time::DateTime getReservationOpeningTime ( 
				const graph::ServicePointer& servicePointer
			) const;
			
			
			
			/** Indicates whether or not a path can be taken at a given date, 
				taking into account reservation delay rules.
				@return true if the line run can be taken, false otherwise.

				This methods checks the following conditions :
					- if reservation is not compulsory, the run can be taken.
					- if reservation is compulsory, reservation time must precede reservation 
				dead line and be after reservation opening time.
			*/
			virtual RunPossibilityType isRunPossible (
				const graph::ServiceUse& serviceUse
			) const;


			virtual RunPossibilityType isRunPossible (
				const graph::ServicePointer& servicePointer
			) const;


			/** Indicates whether or not a reservation is possible for a given run,
				at a certain date, taking into account delay rules.
				@return true if the reservation is possible, false otherwise.
			 
				This methods checks the following conditions :
					- reservation time must precede reservation dead line
					- reservation time must be later than reservation start time.
			*/
			virtual ReservationAvailabilityType getReservationAvailability(
				const graph::ServiceUse& serviceUse
			) const;


			//////////////////////////////////////////////////////////////////////////
			/// Temporary reservation availability getter.
			/// 
			/// @warning The reservation availability result produced by this method
			/// is temporary : the departure time
			/// must be known to determinate the reservation availability definitely.
			/// In ARRIVAL_TO_DEPARTURE method, the arrival time is the only one
			/// known. The temporary result is based on the arrival time : if no
			/// reservation can be done for the arrival time, it is not possible to do
			/// one for any departure time, that is necessarily earlier. So the
			/// result of this method MUST be confirmed by
			/// getReservationAvailability(const ServiceUse&, ...)
			virtual ReservationAvailabilityType getReservationAvailability(
				const graph::ServicePointer& servicePointer
			) const;


			virtual bool isCompatibleWith(
				const graph::AccessParameters& accessParameters
			) const;

		};
	}
}

#endif // SYNTHESE_PTUseRule_h__
