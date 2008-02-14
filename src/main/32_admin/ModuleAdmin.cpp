
/** ModuleAdmin class implementation.
	@file ModuleAdmin.cpp
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

#include "ModuleAdmin.h"
#include "HomeAdmin.h"

#include "01_util/ModuleClass.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace admin;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ModuleAdmin>::FACTORY_KEY("ModuleAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ModuleAdmin>::ICON("brick.png");
		template<> const string AdminInterfaceElementTemplate<ModuleAdmin>::DEFAULT_TITLE("Module inconnu");
	}

	namespace admin
	{
		const string ModuleAdmin::PARAMETER_MODULE("mod");

		ModuleAdmin::ModuleAdmin()
			: AdminInterfaceElementTemplate<ModuleAdmin>()
		{ }
		
		void ModuleAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_moduleKey = map.getString(PARAMETER_MODULE, true, FACTORY_KEY);
			_pageLink.name = _moduleKey;	/// @todo temporary
			_pageLink.parameterName = PARAMETER_MODULE;
			_pageLink.parameterValue = _moduleKey;
		}
		
		void ModuleAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the display by streaming the output to the stream variable
		}

		bool ModuleAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks ModuleAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == HomeAdmin::FACTORY_KEY)
			{
				for (Factory<ModuleClass>::Iterator it = Factory<ModuleClass>::begin(); 
					it != Factory<ModuleClass>::end(); ++it)
				{ /// @todo Add a control of presence of at least a subpage for the module
					AdminInterfaceElement::PageLink link;
					link.factoryKey = FACTORY_KEY;
					link.icon = ICON;
					link.parameterValue = it.getKey();
					link.parameterName = PARAMETER_MODULE;
					link.name = it.getKey();	/// @todo Replace it by a name
					links.push_back(link);
				}
			}
			return links;
		}
		
	}
}
