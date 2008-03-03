
/** ResaCustomersAdmin class implementation.
	@file ResaCustomersAdmin.cpp
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

#include "ResaCustomersAdmin.h"
#include "ResaModule.h"

#include "30_server/QueryString.h"

#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaCustomersAdmin>::FACTORY_KEY("ResaCustomersAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaCustomersAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<ResaCustomersAdmin>::DEFAULT_TITLE("Clients");
	}

	namespace resa
	{
		ResaCustomersAdmin::ResaCustomersAdmin()
			: AdminInterfaceElementTemplate<ResaCustomersAdmin>()
		{ }
		
		void ResaCustomersAdmin::setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
			// Exception example:
			// throw AdminParametersException("Parameter not found");
			// Example : _pageLink.name=object.getName();
			// Example : _pageLink.parameterName=QueryString::PARAMETER_OBJECT_ID;
			// Example : _pageLink.parameterValue=Conversion::ToString(id);
		}
		
		void ResaCustomersAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the display by streaming the output to the stream variable
		}

		bool ResaCustomersAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the right control;
			return false;
		}
		
		AdminInterfaceElement::PageLinks ResaCustomersAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			/// @todo Implement it or leave empty
			// Example
			// if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == ResaModule::FACTORY_KEY)
			//	links.push_back(getPageLink());
			return links;
		}
	}
}
