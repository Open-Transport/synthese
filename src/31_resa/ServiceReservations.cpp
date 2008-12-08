
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

#include "15_env/Service.h"


#include "31_resa/Reservation.h"
#include "31_resa/ReservationTransaction.h"

namespace synthese
{
	namespace resa
	{


		boost::shared_ptr<Reservation> ServiceReservations::getReservation(
			const ReservationTransaction* transaction
		) const	{
			const ReservationTransaction::Reservations& r(transaction->getReservations());
			for (ReservationTransaction::Reservations::const_iterator ite(r.begin()); ite != r.end(); ++ite)
				if ((*ite)->getServiceId() == service->getKey())
					return *ite;
		}

		ServiceReservations::ServiceReservations()
			: service(NULL)
			, seatsNumber(0)
			, overflow(false)
			, status(false)	
		{

		}
	}
}
