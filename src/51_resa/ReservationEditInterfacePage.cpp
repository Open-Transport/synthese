
/** ReservationEditInterfacePage class implementation.
	@file ReservationEditInterfacePage.cpp
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

#include "ReservationEditInterfacePage.h"

#include "Interface.h"
#include "ResaModule.h"
#include "Profile.h"
#include "ReservationTransaction.h"
#include "Reservation.h"
#include "ReservationConfirmationEMailItemInterfacePage.h"
#include "Request.h"
#include "ResaRight.h"
#include "Session.h"
#include "User.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace resa;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationEditInterfacePage>::FACTORY_KEY("reservation_edit");
	}

	namespace resa
	{
		ReservationEditInterfacePage::ReservationEditInterfacePage()
			: Registrable(0),
			  FactorableTemplate<interfaces::InterfacePage, ReservationEditInterfacePage>()
		{
		}



		void ReservationEditInterfacePage::display(
			std::ostream& stream,
			const ReservationTransaction& resa,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			pv.push_back(lexical_cast<string>(resa.getKey())); // 1

			if(	request->getSession() &&
				request->getUser() &&
				request->getUser()->getProfile() &&
				(	request->getUser()->getProfile()->isAuthorized<ResaRight>(READ) ||
					(request->getUser()->getProfile()->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, READ) &&
					 resa.getCustomerUserId() == request->getUser()->getKey())
			)	){

				stringstream s;
				if(getInterface()->getPage<ReservationConfirmationEMailItemInterfacePage>())
				{
					BOOST_FOREACH(const Reservation* r, resa.getReservations())
					{
						getInterface()->getPage<ReservationConfirmationEMailItemInterfacePage>()->display(s, *r, variables, request);
				}	}
				pv.push_back(s.str()); // 1

				pv.push_back(lexical_cast<string>(resa.getCustomerUserId())); // 2

				pv.push_back(to_simple_string(resa.getReservationDeadLine().date())); // 3
				pv.push_back(to_simple_string(resa.getReservationDeadLine().time_of_day())); // 4

				pv.push_back((*resa.getReservations().begin())->getDeparturePlaceName()); // 5
				pv.push_back((*resa.getReservations().rbegin())->getArrivalPlaceName()); // 6

				pv.push_back(to_simple_string((*resa.getReservations().begin())->getDepartureTime().date())); // 7

				pv.push_back(resa.getCustomerName()); // 8

				pv.push_back(resa.getCustomerPhone()); // 9

				pv.push_back(ResaModule::GetStatusText(resa.getStatus())); // 10

				pv.push_back(lexical_cast<string>(resa.getReservationDeadLine() > second_clock::local_time())); // 11

				pv.push_back(request->getSession()->getKey()); // 12

				pv.push_back(resa.getCancellationTime().is_not_a_date_time() ? string() : to_simple_string(resa.getCancellationTime().date()));
				pv.push_back(resa.getCancellationTime().is_not_a_date_time() ? string() : to_simple_string(resa.getCancellationTime().time_of_day())); //14
			} else {
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
			}

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
