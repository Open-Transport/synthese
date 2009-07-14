
/** ServiceReservations class implementation.
	@file ServiceReservations.cpp

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

#include "ServiceReservations.h"

#include "Reservation.h"
#include "ReservationTransaction.h"

#include <boost/foreach.hpp>

using namespace boost;

namespace synthese
{
	namespace resa
	{


/*		boost::shared_ptr<Reservation> ServiceReservations::getReservation(
			const ReservationTransaction* transaction
		) const	{
			const ReservationTransaction::Reservations& r(transaction->getReservations());
			for (ReservationTransaction::Reservations::const_iterator ite(r.begin()); ite != r.end(); ++ite)
				if ((*ite)->getServiceId() == service->getKey())
					return *ite;
		}
*/



		bool ServiceReservations::ReservationsLess::operator()( boost::shared_ptr<const Reservation> left, boost::shared_ptr<const Reservation> right ) const
		{
			return
				left->getDepartureTime() < right->getDepartureTime() ||
				(	left->getDepartureTime() == right->getDepartureTime() &&
					left->getArrivalTime() < right->getArrivalTime()
				) || (
					left->getDepartureTime() == right->getDepartureTime() &&
					left->getArrivalTime() == right->getArrivalTime() &&
					left.get() < right.get()
				)
			;
		}

		void ServiceReservations::addReservation( boost::shared_ptr<const Reservation> reservation )
		{
			_reservations.insert(reservation);
		}

		const ServiceReservations::ReservationsList& ServiceReservations::getReservations() const
		{
			return _reservations;
		}

		int ServiceReservations::getSeatsNumber() const
		{
			int result(0);
			BOOST_FOREACH(shared_ptr<const Reservation> resa, _reservations)
			{
				if (resa->getTransaction()->getCancellationTime().isUnknown())
				{
					result += resa->getTransaction()->getSeats();
				}
			}
			return result;
		}
	}
}
