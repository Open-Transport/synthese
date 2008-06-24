
/** ReservationItemInterfacePage class implementation.
	@file ReservationItemInterfacePage.cpp
	@author Hugues Romain
	@date 2008

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

#include "ReservationItemInterfacePage.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTransaction.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationItemInterfacePage>::FACTORY_KEY("reservation_transaction");
	}

	namespace resa
	{

		void ReservationItemInterfacePage::display(
			std::ostream& stream
			, const ReservationTransaction& transaction
			, VariablesMap& variables
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			
			DateTime now(TIME_CURRENT);

			pv.push_back((*transaction.getReservations().begin())->getDeparturePlaceName());
			pv.push_back((*transaction.getReservations().begin())->getDepartureTime().toString());
			pv.push_back((*(transaction.getReservations().end() - 1))->getArrivalPlaceName());
			pv.push_back((*(transaction.getReservations().end() - 1))->getArrivalTime().toString());
			pv.push_back(Conversion::ToString(transaction.getSeats()));
			if (transaction.getCancellationTime().isUnknown() && now <= transaction.getReservationDeadLine())
				pv.push_back(Conversion::ToString(transaction.getKey()));
			else
				pv.push_back(string());
			pv.push_back(Conversion::ToString(static_cast<int>(transaction.getStatus())));
			
			InterfacePage::display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&transaction)
				, request
			);
		}



		ReservationItemInterfacePage::ReservationItemInterfacePage()
			: util::FactorableTemplate<interfaces::InterfacePage, ReservationItemInterfacePage>()
		{

		}
	}
}
