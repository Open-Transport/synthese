
/** ReservationTransaction class implementation.
	@file ReservationTransaction.cpp

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

#include "ReservationTransaction.h"

#include "31_resa/Reservation.h"

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace resa
	{


		ReservationTransaction::ReservationTransaction()
			: Registrable<uid,ReservationTransaction>()
			, _bookingTime(TIME_CURRENT)
			, _cancellationTime(TIME_UNKNOWN)
			, _originDateTime(TIME_UNKNOWN)
			, _destinationDateTime(TIME_UNKNOWN)
		{

		}

		void ReservationTransaction::setLastReservation( uid id )
		{
			_lastReservation = id;
		}

		void ReservationTransaction::setSeats( int seats )
		{
			_seats = seats;
		}

		void ReservationTransaction::setBookingTime( const time::DateTime& time )
		{
			_bookingTime = time;
		}

		void ReservationTransaction::setCancellationTime( const time::DateTime& time )
		{
			_cancellationTime = time;
		}

		void ReservationTransaction::setCustomerUserId( uid id )
		{
			_customerUserId = id;
		}

		void ReservationTransaction::setCustomerName( const std::string& name )
		{
			_customerName = name;
		}

		void ReservationTransaction::setCustomerPhone( const std::string& phone )
		{
			_customerPhone = phone;
		}

		void ReservationTransaction::setBookingUserId( uid id )
		{
			_bookingUserId = id;
		}

		void ReservationTransaction::setCancelUserId( uid id )
		{
			_cancelUserId = id;
		}

		uid ReservationTransaction::getLastReservation() const
		{
			return _lastReservation;
		}

		int ReservationTransaction::getSeats() const
		{
			return _seats;
		}

		const time::DateTime& ReservationTransaction::getBookingTime() const
		{
			return _bookingTime;
		}

		const time::DateTime& ReservationTransaction::getCancellationTime() const
		{
			return _cancellationTime;
		}

		uid ReservationTransaction::getCustomerUserId() const
		{
			return _customerUserId;
		}

		const std::string& ReservationTransaction::getCustomerName() const
		{
			return _customerName;
		}

		const std::string& ReservationTransaction::getCustomerPhone() const
		{
			return _customerPhone;
		}

		uid ReservationTransaction::getBookingUserId() const
		{
			return _bookingUserId;
		}

		uid ReservationTransaction::getCancelUserId() const
		{
			return _cancelUserId;
		}

		void ReservationTransaction::setCustomerEMail( const std::string& email )
		{
			_customerEMail = email;
		}

		const std::string& ReservationTransaction::getCustomerEMail() const
		{
			return _customerEMail;
		}

		void ReservationTransaction::addReservation(Reservation* reservation )
		{
			reservation->setTransaction(this);
			_reservations.push_back(reservation);
		}

		const ReservationTransaction::Reservations& ReservationTransaction::getReservations() const
		{
			return _reservations;
		}
	}
}
