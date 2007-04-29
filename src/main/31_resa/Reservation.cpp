
/** Reservation class implementation.
	@file Reservation.cpp

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

#include "Reservation.h"

namespace synthese
{
	namespace resa
	{


		void Reservation::setLineId( uid id )
		{
			_lineId = id;
		}

		void Reservation::setLineCode( const std::string& code )
		{
			_lineCode = code;
		}

		void Reservation::setServiceCode( const std::string& code )
		{
			_serviceCode = code;
		}

		void Reservation::setDeparturePlaceId( uid id )
		{
			_departurePlaceId = id;
		}

		void Reservation::setDeparturePlaceName( const std::string& name )
		{
			_departurePlaceName = name;
		}

		void Reservation::setArrivalPlaceId( uid id )
		{
			_arrivalPlaceId = id;
		}

		void Reservation::setArrivalPlaceName( const std::string& name )
		{
			_arrivalPlaceName = name;
		}

		void Reservation::setReservationRuleId( uid id )
		{
			_reservationRuleId = id;
		}

		void Reservation::setArrivalAddress( const std::string& address )
		{
			_arrivalAddress = address;
		}

		void Reservation::setDepartureAddress( const std::string& address )
		{
			_departureAddress = address;
		}

		void Reservation::setDepartureTime( const time::DateTime& time )
		{
			_departureTime = time;
		}

		void Reservation::setArrivalTime( const time::DateTime& time )
		{
			_arrivalTime = time;
		}

		void Reservation::setLastReservation( uid id )
		{
			_lastReservation = id;
		}

		void Reservation::setSeats( int seats )
		{
			_seats = seats;
		}

		void Reservation::setBookingTime( const time::DateTime& time )
		{
			_bookingTime = time;
		}

		void Reservation::setCancellationTime( const time::DateTime& time )
		{
			_cancellationTime = time;
		}

		void Reservation::setCustomerUserId( uid id )
		{
			_customerUserId = id;
		}

		void Reservation::setCustomerName( const std::string& name )
		{
			_customerName = name;
		}

		void Reservation::setCustomerPhone( const std::string& phone )
		{
			_customerPhone = phone;
		}

		void Reservation::setBookingUserId( uid id )
		{
			_bookingUserId = id;
		}

		void Reservation::setCancelUserId( uid id )
		{
			_cancelUserId = id;
		}
	}
}
