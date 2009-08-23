
/** ReservationCancellationEMailContentInterfacePage class implementation.
	@file ReservationCancellationEMailContentInterfacePage.cpp
	@author Hugues
	@date 2009

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

#include "ReservationCancellationEMailContentInterfacePage.h"
#include "ReservationTransaction.h"
#include "ReservationConfirmationEMailItemInterfacePage.h"
#include "Reservation.h"
#include "Interface.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace resa;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationCancellationEMailContentInterfacePage>::FACTORY_KEY("reservation_cancellation_email_content");
	}

	namespace resa
	{
		ReservationCancellationEMailContentInterfacePage::ReservationCancellationEMailContentInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, ReservationCancellationEMailContentInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void ReservationCancellationEMailContentInterfacePage::display(
			std::ostream& stream,
			const ReservationTransaction& resa,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			stringstream s;
			if(getInterface()->getPage<ReservationConfirmationEMailItemInterfacePage>())
			{
				BOOST_FOREACH(const Reservation* r, resa.getReservations())
				{
					getInterface()->getPage<ReservationConfirmationEMailItemInterfacePage>()->display(s, *r, variables, request);
			}	}
			pv.push_back(s.str()); // 0

			pv.push_back(lexical_cast<string>(resa.getKey())); // 1

			pv.push_back(lexical_cast<string>(resa.getCustomerUserId())); // 2

			pv.push_back(resa.getReservationDeadLine().getDate().toString()); // 3
			pv.push_back(resa.getReservationDeadLine().getHour().toString()); // 4

			pv.push_back((*resa.getReservations().begin())->getDeparturePlaceName()); // 5
			pv.push_back((*(resa.getReservations().end()-1))->getArrivalPlaceName()); // 6

			pv.push_back((*resa.getReservations().begin())->getDepartureTime().getDate().toString()); // 7

			pv.push_back(resa.getCustomerName()); // 8

			pv.push_back(resa.getCustomerPhone()); // 9

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&resa)
				, request
			);
		}
	}
}
