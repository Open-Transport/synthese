////////////////////////////////////////////////////////////////////////////////
/// UseRule class header.
///	@file UseRule.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace graph
	{
		class ServicePointer;
		class AccessParameters;

		//////////////////////////////////////////////////////////////////////////
		/// Use rule class.
		///
		/// A use rule indicates if a customer profile is allowed to use a service
		/// regarding some criteria.
		///
		/// For example, a service can be used by non handicapped persons without
		/// any constraint, two handicapped persons can use the service, and
		/// persons with bikes can use the service if they have booked it two
		/// hours earlier.
		/// This case is implemented by 3 use rules objects (one per customer
		/// profile)
		///
		/// UseRule is an interface. Each implementation of the graph module must
		/// implement it at least once.
		///
		/// When no use rule is specified for a service and a customer profile,
		/// it seems that the access to the service is forbidden for the profile
		/// members. It is equivalent to the FORBIDDEN_USE_RULE static instance
		/// of UseRule.
		///
		/// @ingroup m18
		class UseRule
		{
		public:


			typedef enum
			{
				RESERVATION_FORBIDDEN = 0,
				RESERVATION_COMPULSORY_POSSIBLE = 1,
				RESERVATION_COMPULSORY_TOO_EARLY = 2,
				RESERVATION_COMPULSORY_TOO_LATE = 3,
				RESERVATION_OPTIONAL_POSSIBLE = 4,
				RESERVATION_OPTIONAL_TOO_EARLY = 5,
				RESERVATION_OPTIONAL_TOO_LATE = 6,
				RESERVATION_DEPENDING_ON_DEPARTURE_PLACE = 7
			} ReservationAvailabilityType;


			typedef enum
			{
				RUN_NOT_POSSIBLE = 0,
				RUN_POSSIBLE = 1,
				RUN_DEPENDING_ON_DEPARTURE_PLACE = 2
			} RunPossibilityType;

			
			typedef enum
			{
				RESERVATION_INTERNAL_DELAY = 0,
				RESERVATION_EXTERNAL_DELAY = 1
			} ReservationDelayType;

			//////////////////////////////////////////////////////////////////////////
			/// Maximal seats number for the user category.
			/// Values :
			///  - 0 : the use of the service is forbidden for the user category;
			///  - >0 : the use of the service is allowed for the user category, the
			///    available seats number is limited to the specified value;
			///  - unspecified : the use of the service is allowed for the user category,
			///    and there is no seats number limit.
			typedef boost::optional<size_t> AccessCapacity;


			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Pure virtual name getter.
				/// @return name of the use rule
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual std::string getUseRuleName() const = 0;

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
				virtual boost::posix_time::ptime getReservationDeadLine (
					const boost::posix_time::ptime& originTime,
					const boost::posix_time::ptime& departureTime,
					const ReservationDelayType& reservationRuleDelayType = RESERVATION_INTERNAL_DELAY
				) const = 0;


				virtual AccessCapacity getAccessCapacity(
				) const = 0;


				/** Reference function for calculation of the date time of the opening of reservations.
					@param reservationTime Time when booking is done.
					@return The minimum date time to make a reservation.

					If no explicit rule defines this minimum time, the actual reservation time is returned.
				*/
				virtual boost::posix_time::ptime getReservationOpeningTime (
					const ServicePointer& servicePointer
				) const = 0;



				/** Indicates whether or not a path can be taken at a given date,
					taking into account reservation delay rules.
					@return true if the line run can be taken, false otherwise.

					This methods checks the following conditions :
						- if reservation is not compulsory, the run can be taken.
						- if reservation is compulsory, reservation time must precede reservation
					dead line and be after reservation opening time.
				*/
				virtual RunPossibilityType isRunPossible (
					const ServicePointer& servicePointer,
					bool ignoreReservation,
					int reservationRulesDelayType = 0
				) const = 0;


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
				virtual ReservationAvailabilityType getReservationAvailability(
					const ServicePointer& servicePointer,
					bool ignoreReservationDeadline,
					int reservationRulesDelayType = 0
				) const = 0;


				virtual bool isCompatibleWith(
					const AccessParameters& accessParameters
				) const = 0;

				static bool IsReservationPossible(
					const ReservationAvailabilityType& value
				);
			//@}

			UseRule();
			~UseRule();
		};
	}
}

#endif

