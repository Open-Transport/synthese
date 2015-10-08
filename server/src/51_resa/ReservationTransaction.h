
/** ReservationTransaction class header.
	@file ReservationTransaction.h

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

#ifndef SYNTHESE_resa_ReservationTransaction_h__
#define SYNTHESE_resa_ReservationTransaction_h__

#include "Object.hpp"

#include "PtimeField.hpp"
#include "ResaTypes.h"
#include "StringField.hpp"

#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace resa
	{
		class Reservation;

		FIELD_ID(LastReservationId)
		FIELD_SIZE_T(Seats)
		FIELD_PTIME(BookingTime)
		FIELD_PTIME(CancellationTime)
		FIELD_POINTER(Customer, security::User)
		FIELD_STRING(CustomerName)
		FIELD_STRING(CustomerPhone)
		FIELD_STRING(CustomerEmail)
		FIELD_ID(BookingUserId)
		FIELD_ID(CancelUserId)
		FIELD_STRING(Comment)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(LastReservationId),
			FIELD(Seats),
			FIELD(BookingTime),
			FIELD(CancellationTime),
			FIELD(Customer),
			FIELD(CustomerName),
			FIELD(CustomerPhone),
			FIELD(CustomerEmail),
			FIELD(BookingUserId),
			FIELD(CancelUserId),
			FIELD(Comment)
		> ReservationTransactionSchema;

		/** ReservationTransaction class.
			@ingroup m51
		*/
		class ReservationTransaction
		:	public virtual Object<ReservationTransaction, ReservationTransactionSchema>
		{
		public:

			struct ReservationComparator
			{
				bool operator()(Reservation* op1, Reservation* op2);
			};

			typedef std::set<Reservation*, ReservationComparator> Reservations;

		private:

			//!	\name Reservation attributes
			//@{
				Reservations		_reservations;
			//@}

			//! \name Journey
			//@{
				util::RegistryKeyType	_originPlaceId;
				std::string			_originCityText;
				std::string			_originPlaceTextNoCity;
				std::string			_originPlaceText; // This concatenates place and city
				util::RegistryKeyType	_destinationPlaceId;
				std::string			_destinationPlaceText;
				boost::posix_time::ptime		_originDateTime;
				boost::posix_time::ptime		_destinationDateTime;
				std::string			_htmlJourneyBoard;
			//@}

		public:
			ReservationTransaction(util::RegistryKeyType key = 0);
			~ReservationTransaction();

			const Reservations&		getReservations()		const;



			//////////////////////////////////////////////////////////////////////////
			/// Adds a reservation in the transaction.
			/// @param resa the reservation to add
			/// @pre the reservation to add must belong to the current transaction
			void addReservation(Reservation& resa);



			//////////////////////////////////////////////////////////////////////////
			/// Removes a reservation from the transaction.
			/// @param resa the reservation to remove
			void removeReservation(Reservation& resa);

			//! @name Queries
			//@{
				/** Reservation transaction status computing.
					@return ReservationStatus the status of the reservation transaction
					@author Hugues Romain
					@date 2008

					The reservation status of a reservation transaction is :
						- NO_SHOW if at least one reservation is at NO_SHOW status
						- else the minimum of the status of each reservation
				*/
				ReservationStatus getStatus() const;

				std::string	getFullStatusText()	const;



				/** Dead line of the reservation transaction.
					@return the deadline
					@author Hugues Romain
					@date 2008

					The dead line of the reservation transaction is the time when the cancellation is not allowed anymore.
					This is the first reservation dead line in chronological order.
				*/
				boost::posix_time::ptime getReservationDeadLine() const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_resa_ReservationTransaction_h__
