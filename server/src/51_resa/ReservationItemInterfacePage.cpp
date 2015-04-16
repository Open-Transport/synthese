
/** ReservationItemInterfacePage class implementation.
	@file ReservationItemInterfacePage.cpp
	@author Hugues Romain
	@date 2008

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

#include "ReservationItemInterfacePage.h"
#include "Reservation.h"
#include "ReservationTransaction.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationItemInterfacePage>::FACTORY_KEY("reservation_transaction");
	}

	namespace resa
	{
		const string ReservationItemInterfacePage::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string ReservationItemInterfacePage::DATA_DEPARTURE_TIME("departure_time");
		const string ReservationItemInterfacePage::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const string ReservationItemInterfacePage::DATA_ARRIVAL_TIME("arrival_time");
		const string ReservationItemInterfacePage::DATA_SEATS("seats");
		const string ReservationItemInterfacePage::DATA_URL_TO_CANCELLATION("url_to_cancellation");
		const string ReservationItemInterfacePage::DATA_STATUS_CANCELLED("status_cancelled");

		void ReservationItemInterfacePage::display(
			std::ostream& stream
			, const ReservationTransaction& transaction
			, VariablesMap& variables
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			ptime now(second_clock::local_time());

			pv.push_back((*transaction.getReservations().begin())->getDeparturePlaceName());
			pv.push_back(to_simple_string((*transaction.getReservations().begin())->getDepartureTime()));
			pv.push_back((*transaction.getReservations().rbegin())->getArrivalPlaceName());
			pv.push_back(to_simple_string((*transaction.getReservations().rbegin())->getArrivalTime()));
			pv.push_back(lexical_cast<string>(transaction.getSeats()));
			if (transaction.getCancellationTime().is_not_a_date_time() && now <= transaction.getReservationDeadLine())
				pv.push_back(lexical_cast<string>(transaction.getKey()));
			else
				pv.push_back(string());
			pv.push_back(lexical_cast<string>(static_cast<int>(transaction.getStatus())));

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&transaction)
				, request
			);
		}



		ReservationItemInterfacePage::ReservationItemInterfacePage()
			: Registrable(0),
			  util::FactorableTemplate<interfaces::InterfacePage, ReservationItemInterfacePage>()			
		{

		}
	}
}
