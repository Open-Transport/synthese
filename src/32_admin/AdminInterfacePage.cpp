
/** AdminInterfacePage class implementation.
	@file AdminInterfacePage.cpp

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

#include "AdminInterfaceElement.h"
#include "AdminInterfacePage.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	template<> const std::string util::FactorableTemplate<interfaces::InterfacePage,admin::AdminInterfacePage>::FACTORY_KEY("admin");

	namespace admin
	{
		const string AdminInterfacePage::DATA_USER_FULL_NAME("user_full_name");
		
		void AdminInterfacePage::display(
			std::ostream& stream,
			const AdminInterfaceElement* page,
			const boost::optional<std::string>& errorMessage,
			const server::Request* request /*= NULL */
		) const	{
			ParametersVector parameters;
			parameters.push_back(
				request->getSession() && request->getUser().get() ?
				request->getUser()->getFullName() :
				string()
			);
			parameters.push_back(errorMessage ? *errorMessage : string());
			
			VariablesMap vars;

			InterfacePage::_display(
				stream,
				parameters,
				vars,
				static_cast<const void*>(page),
				request
			);
		}



		AdminInterfacePage::AdminInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
