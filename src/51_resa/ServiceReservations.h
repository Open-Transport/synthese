
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
#include <set>

#include "Env.h"

namespace synthese
{
	namespace resa
	{
		class Reservation;

		/** Container for ordered reservations list of a service class.
			@ingroup m31
		*/
		class ServiceReservations
		{
		public:
			struct ReservationsLess : public std::binary_function<boost::shared_ptr<const Reservation>, boost::shared_ptr<const Reservation>, bool>
			{
				bool operator()(boost::shared_ptr<const Reservation> left, boost::shared_ptr<const Reservation> right) const;
			};

			typedef std::set<boost::shared_ptr<const Reservation>, ReservationsLess> ReservationsList;
			

		private:
			ReservationsList	_reservations;

		public:
			void addReservation(boost::shared_ptr<const Reservation> reservation);
			const ReservationsList& getReservations() const;

			int getSeatsNumber() const;
		};
	}
}

#endif // SYNTHESE_resa_ServiceReservations_h__
