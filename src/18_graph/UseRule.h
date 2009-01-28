////////////////////////////////////////////////////////////////////////////////
/// UseRule class header.
///	@file UseRule.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_UseRule_h__
#define SYNTHESE_UseRule_h__

#include "DateTime.h"

namespace synthese
{
	namespace graph
	{
		////
		/// Use rule class.
		/// @ingroup m18
		class UseRule
		{
		public:
			////
			/// Reservation rule type.
			///	- FORBIDDEN : it is impossible to book a seat on the service
			///	- COMPULSORY : it is impossible to use the service without having booked a seat
			///	- OPTIONAL : is is possible to book a place on the service, but it is possible to use 
			///   the service without having booked a seat
			///	- MIXED_BY_DEPARTURE_PLACE : it is impossible to use the service without having booked 
			///   a place, except for journeys beginning at several places, defined in the path.
			typedef enum {
				RESERVATION_FORBIDDEN = 0,
				RESERVATION_COMPULSORY = 1,
				RESERVATION_OPTIONAL = 2,
				RESERVATION_MIXED_BY_DEPARTURE_PLACE = 3
			} ReservationRuleType;
			
			
			////
			/// Access level.
			/// The rule is applicable on a user class (see @ref UseRules ) :
			///  - FORBIDDEN : The access is not allowed to the user class members
			///  - ALLOWED : The access is allowed to the user class members
			///  - COMPULSORY : The access is allowed only to the user class members
			typedef enum
			{
				ACCESS_FORBIDDEN = 0,
				ACCESS_ALLOWED = 1,
				ACCESS_COMPULSORY = 2,
				ACCESS_UNKNOWN = 3
			} Access;

			typedef unsigned int Capacity;
			
			static const Capacity UNLIMITED_CAPACITY;

			static const UseRule ALLOWED;
			static const UseRule FORBIDDEN;
			static const UseRule UNKNOWN;

		private:
			
			//! @name Access
			//@{
				////
				/// Value (see the derivated class for the signification of the 3 availables status)
				Access _access;
				
				////
				/// Maximal person number which can be served
				/// The maximum value of the attribute seems unlimited capacity
				Capacity _capacity;
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
			UseRule(
				Access access = ACCESS_UNKNOWN
			);
			~UseRule();
		
			//! @name Getters
			//@{
				Capacity			getCapacity() const;
				Access				getAccess() const;
				bool				getOriginIsReference()			const;
				const time::Hour&	getHourDeadLine()				const;
				int					getMinDelayDays()				const;
				int					getMinDelayMinutes()			const;
				int					getMaxDelayDays()				const;
				ReservationRuleType	getReservationType()			const;
			//@}
			
			//! @name Setters
			//@{
				void setHourDeadLine (const synthese::time::Hour& hourDeadLine);
				void setMinDelayMinutes (int minDelayMinutes);
				void setMinDelayDays (int minDelayDays);
				void setMaxDelayDays (int maxDelayDays);
				void setOriginIsReference (bool originIsReference);
				void setReservationType(ReservationRuleType value);
			//@}

			//! @name Queries
			//@{
			
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
				time::DateTime getReservationDeadLine (
					const time::DateTime& originTime
					, const time::DateTime& departureTime
				) const;
				
				
				
				/** Reference function for calculation of start reservation date time.
					@param reservationTime Time when booking is done.
					@return The minimum date time to make a reservation.
				
					If no explicit rule defines this minimum time, the actual reservation time is returned.
				*/
				time::DateTime getReservationStartTime ( 
					const time::DateTime& reservationTime
				) const;
				
				
				
				/** Indicates whether or not a path can be taken at a given date, 
					taking into account reservation delay rules.
					@param originTime Time of start of the corresponding service
					@param reservationTime Time of booking, if required.
					@param departureTime Desired departure time.
					@return true if the line run can be taken, false otherwise.

					This methods checks the following conditions :
						- if reservation is not compulsory, the run can be taken.
						- if reservation is compulsory, reservation time must precede reservation 
					dead line and be after reservation opening time.
				*/
				bool isRunPossible (
					const time::DateTime& originTime,
					bool stopBelongsToOptionalReservationPlaces,
					const time::DateTime& reservationTime,
					const time::DateTime& departureTime
				) const;



				/** Indicates whether or not a reservation is possible for a given run,
					at a certain date, taking into account delay rules.
					@param originTime Time of start of the corresponding service
					@param reservationTime Time of booking.
					@param departureTime Desired departure time.
					@return true if the reservation is possible, false otherwise.
				 
					This methods checks the following conditions :
						- reservation time must precede reservation dead line
						- reservation time must be later than reservation start time.
				*/
				bool isReservationPossible (
					const time::DateTime& originTime
					, const time::DateTime& reservationTime
					, const time::DateTime& departureTime
				) const;

			//@}
		};
	}
}

#endif