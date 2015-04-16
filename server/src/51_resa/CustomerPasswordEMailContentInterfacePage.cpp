
/** CustomerPasswordEMailContentInterfacePage class implementation.
	@file CustomerPasswordEMailContentInterfacePage.cpp
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

#include "CustomerPasswordEMailContentInterfacePage.h"
#include "User.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, resa::CustomerPasswordEMailContentInterfacePage>::FACTORY_KEY("customer_password_email_content");
	}

	namespace resa
	{
		CustomerPasswordEMailContentInterfacePage::CustomerPasswordEMailContentInterfacePage()
			: Registrable(0),
			  FactorableTemplate<interfaces::InterfacePage, CustomerPasswordEMailContentInterfacePage>()
		{
		}



		void CustomerPasswordEMailContentInterfacePage::display(
			std::ostream& stream,
			const User& user,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			pv.push_back(user.getSurname());
			pv.push_back(user.getName());
			pv.push_back(lexical_cast<string>(user.getKey()));
			pv.push_back(user.getPhone());
			pv.push_back(user.getEMail());
			pv.push_back(user.getLogin());
			if(!user.getPassword().empty())
			{
				pv.push_back(user.getPassword());
			}
			else
			{
				pv.push_back("Mot de passe inchangé");
			}


			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&user)
				, request
			);
		}
	}
}
