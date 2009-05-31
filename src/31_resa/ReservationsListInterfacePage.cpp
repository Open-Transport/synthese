
/** ReservationsListInterfacePage class implementation.
	@file ReservationsListInterfacePage.cpp
	@author Hugues Romain
	@date 2007

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

#include "ReservationsListInterfacePage.h"

#include "CommercialLine.h"
#include "User.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationsListInterfacePage>::FACTORY_KEY("reservations_list");
	}

	namespace resa
	{
		const string ReservationsListInterfacePage::DATA_LINE_ID("line_id");
		const string ReservationsListInterfacePage::DATA_CUSTOMER_ID("customer_id");
		const string ReservationsListInterfacePage::DATA_RESERVATION_NAME("reservation_name");
		const string ReservationsListInterfacePage::DATA_DATE("date");
		const string ReservationsListInterfacePage::DATA_WITH_CANCELLED_RESERVATIONS("with_cancelled");
		const string ReservationsListInterfacePage::DATA_LINE_NAME("line_name");

		void ReservationsListInterfacePage::display(
			std::ostream& stream
			, boost::shared_ptr<const env::CommercialLine> line
			, boost::shared_ptr<const security::User> user
			, const std::string& userName
			, const Date& date
			, bool withCanceledReservations
			, VariablesMap& variables
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			
			pv.push_back(line.get() ? Conversion::ToString(line->getKey()) : string());
			pv.push_back(user.get() ? Conversion::ToString(user->getKey()) : string());
			pv.push_back(userName);
			pv.push_back(date.toSQLString(false));
			pv.push_back(Conversion::ToString(withCanceledReservations));
			pv.push_back(line.get() ? line->getShortName() : string());

			InterfacePage::_display(
				stream
				, pv
				, variables
				, NULL
				, request
			);
		}

		ReservationsListInterfacePage::ReservationsListInterfacePage()
		:	Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
