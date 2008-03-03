
/** ServiceReservations class header.
	@file ServiceReservations.h

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

#ifndef SYNTHESE_resa_ServiceReservations_h__
#define SYNTHESE_resa_ServiceReservations_h__

#include <boost/shared_ptr.hpp>
#include <vector>

namespace synthese
{
	namespace env
	{
		class Service;
	}

	namespace resa
	{
		class Reservation;
		class ReservationTransaction;

		/** ServiceReservations class.
			@ingroup m31
		*/
		class ServiceReservations
		{
		public:
			typedef std::vector<boost::shared_ptr<ReservationTransaction> > Reservations;

			const env::Service*			service;
			Reservations			reservations;
			int						seatsNumber;
			bool					overflow;
			bool					status;

			ServiceReservations();

			boost::shared_ptr<Reservation>	getReservation(
				const ReservationTransaction* transaction
			) const;
		};
	}
}

#endif // SYNTHESE_resa_ServiceReservations_h__
