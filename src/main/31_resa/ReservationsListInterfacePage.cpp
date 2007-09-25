
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

#include "15_env/CommercialLine.h"

#include "12_security/User.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::ReservationsListInterfacePage>::FACTORY_KEY("reservations_list");
	}

	namespace resa
	{

		void ReservationsListInterfacePage::display(
			std::ostream& stream
			, boost::shared_ptr<const env::CommercialLine> line
			, boost::shared_ptr<const security::User> user
			, const std::string& userName
			, VariablesMap& variables
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			
			pv.push_back(line.get() ? Conversion::ToString(line->getKey()) : string());
			pv.push_back(user.get() ? Conversion::ToString(user->getKey()) : string());
			pv.push_back(userName);

			InterfacePage::display(
				stream
				, pv
				, variables
				, NULL
				, request
			);
		}

		ReservationsListInterfacePage::ReservationsListInterfacePage()
		{

		}
	}
}
