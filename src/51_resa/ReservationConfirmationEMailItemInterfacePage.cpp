
/** ReservationConfirmationEMailItemInterfacePage class implementation.
	@file ReservationConfirmationEMailItemInterfacePage.cpp
	@author Hugues
	@date 2009

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

#include "ReservationConfirmationEMailItemInterfacePage.h"
#include "Reservation.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationConfirmationEMailItemInterfacePage>::FACTORY_KEY("reservation_confirmation_email_item");
	}

	namespace resa
	{
		ReservationConfirmationEMailItemInterfacePage::ReservationConfirmationEMailItemInterfacePage()
			: Registrable(0),
			  FactorableTemplate<interfaces::InterfacePage, ReservationConfirmationEMailItemInterfacePage>()
			
		{
		}



		void ReservationConfirmationEMailItemInterfacePage::display(
			std::ostream& stream,
			const Reservation& resa,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			pv.push_back(to_simple_string(resa.getDepartureTime().time_of_day()));
			pv.push_back(resa.getDeparturePlaceName());
			pv.push_back(to_simple_string(resa.getArrivalTime().time_of_day()));
			pv.push_back(resa.getArrivalPlaceName());
			pv.push_back(resa.getLineCode());

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
