
/** CustomerPasswordEMailSubjectInterfacePage class implementation.
	@file CustomerPasswordEMailSubjectInterfacePage.cpp
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

#include "CustomerPasswordEMailSubjectInterfacePage.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::CustomerPasswordEMailSubjectInterfacePage>::FACTORY_KEY("customer_password_email_subject");
	}

	namespace resa
	{
		CustomerPasswordEMailSubjectInterfacePage::CustomerPasswordEMailSubjectInterfacePage()
			: Registrable(0),
			  FactorableTemplate<interfaces::InterfacePage, CustomerPasswordEMailSubjectInterfacePage>()
		{
		}



		void CustomerPasswordEMailSubjectInterfacePage::display(
			std::ostream& stream,
			const User& customer,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			pv.push_back(customer.getLogin());

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&customer)
				, request
			);
		}
	}
}
