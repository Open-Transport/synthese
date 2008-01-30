
/** VinciAccountAdmin class implementation.
	@file VinciAccountAdmin.cpp
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

#include "VinciAccountAdmin.h"
#include "VinciAccountsAdminInterfaceElement.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace vinci;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VinciAccountAdmin>::FACTORY_KEY("vinci_account");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VinciAccountAdmin>::ICON("cart.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<VinciAccountAdmin>::DISPLAY_MODE(AdminInterfaceElement::DISPLAYED_IF_CURRENT);

		template<> string AdminInterfaceElementTemplate<VinciAccountAdmin>::getSuperior()
		{
			return VinciAccountsAdminInterfaceElement::FACTORY_KEY;
		}
	}

	namespace vinci
	{
		VinciAccountAdmin::VinciAccountAdmin()
			: AdminInterfaceElementTemplate<VinciAccountAdmin>()
		{ }

		void VinciAccountAdmin::setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
			// Exception example:
			// throw AdminParametersException("Parameter not found");
		}

		string VinciAccountAdmin::getTitle() const
		{
			/// @todo Change the title of the page
			return "title";
		}

		void VinciAccountAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the display by streaming the output to the stream variable
		}

		bool VinciAccountAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
	}
}
