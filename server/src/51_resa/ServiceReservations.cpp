
/** ServiceReservations class implementation.
	@file ServiceReservations.cpp

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

#include "ServiceReservations.h"

#include "Reservation.h"
#include "ReservationTransaction.h"

#include <boost/foreach.hpp>

namespace synthese
{
	namespace resa
	{
		bool ServiceReservations::ReservationsLess::operator()(
			const Reservation* left,
			const Reservation* right
		) const	{
			return
				left->get<DepartureTime>() < right->get<DepartureTime>() ||
				(	left->get<DepartureTime>() == right->get<DepartureTime>() &&
					left->get<DeparturePlaceName>() < right->get<DeparturePlaceName>()
				) || (
					left->get<DepartureTime>() == right->get<DepartureTime>() &&
					left->get<DeparturePlaceName>() == right->get<DeparturePlaceName>() &&
					left->get<ArrivalPlaceName>() < right->get<ArrivalPlaceName>()
				) || (
					left->get<DepartureTime>() == right->get<DepartureTime>() &&
					left->get<DeparturePlaceName>() == right->get<DeparturePlaceName>() &&
					left->get<ArrivalPlaceName>() == right->get<ArrivalPlaceName>() &&
					left->get<Transaction>()->get<BookingTime>() < right->get<Transaction>()->get<BookingTime>()
				) || (
					left->get<DepartureTime>() == right->get<DepartureTime>() &&
					left->get<DeparturePlaceName>() == right->get<DeparturePlaceName>() &&
					left->get<ArrivalPlaceName>() == right->get<ArrivalPlaceName>() &&
					left->get<Transaction>()->get<BookingTime>() == right->get<Transaction>()->get<BookingTime>() &&
					left < right
				)
			;
		}



		void ServiceReservations::addReservation(
			const Reservation* reservation
		){
			_reservations.insert(reservation);
		}



		const ServiceReservations::ReservationsList& ServiceReservations::getReservations() const
		{
			return _reservations;
		}



		int ServiceReservations::getSeatsNumber() const
		{
			int result(0);
			BOOST_FOREACH(const Reservation* resa, _reservations)
			{
				if (resa->get<Transaction>()->get<CancellationTime>().is_not_a_date_time())
				{
					result += resa->get<Transaction>()->get<Seats>();
				}
			}
			return result;
		}
}	}
