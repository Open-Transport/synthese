
/** ReservationCancellationEMailSubjectInterfacePage class implementation.
	@file ReservationCancellationEMailSubjectInterfacePage.cpp
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

#include "ReservationCancellationEMailSubjectInterfacePage.h"
#include "ReservationTransaction.h"
#include "Reservation.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace resa;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationCancellationEMailSubjectInterfacePage>::FACTORY_KEY("reservation_cancellation_email_subject");
	}

	namespace resa
	{
		ReservationCancellationEMailSubjectInterfacePage::ReservationCancellationEMailSubjectInterfacePage()
			: Registrable(0),
			  FactorableTemplate<interfaces::InterfacePage, ReservationCancellationEMailSubjectInterfacePage>()
		{
		}



		void ReservationCancellationEMailSubjectInterfacePage::display(
			std::ostream& stream,
			const ReservationTransaction& resa,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			pv.push_back(to_simple_string((*resa.getReservations().begin())->getDepartureTime().date())); // 0
			pv.push_back((*resa.getReservations().begin())->getDeparturePlaceName()); // 1
			pv.push_back((*resa.getReservations().rbegin())->getArrivalPlaceName()); // 2

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
