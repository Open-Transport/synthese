
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

#include "05_html/HTMLModule.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace admin;
	using namespace html;

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
			try
			{
				_moduleKey = map.getString(PARAMETER_MODULE, true, FACTORY_KEY);
				_moduleClass.reset(Factory<ModuleClass>::create(_moduleKey));
			}
			catch(...)
			{
				throw AdminParametersException("Invalid Module Key");
			}
		}
		
		void ModuleAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			stream << "<h1>Informations sur le module</h1>";

			stream << "<ul>";
			stream << "<li>Code : " << _moduleClass->getFactoryKey() << "</li>";
			stream << "<li>Nom : " << _moduleClass->getName() << "</li>";
			stream << "</ul>";

			stream << "<h1>Pages d'administration</h1>";

			stream << "Les liens suivants donnent accès aux pages d'administration du module " << _moduleClass->getName() << ".</p>";
			
			stream << "<ul>";

			AdminInterfaceElement::PageLinks links(getSubPages(*this, request));
			for (AdminInterfaceElement::PageLinks::const_iterator it(links.begin()); it != links.end(); ++it)
			{
				stream << "<li>" << HTMLModule::getHTMLImage(it->icon, it->name);
				stream << HTMLModule::getHTMLLink(it->getURL(static_cast<const server::FunctionRequest<AdminRequest>*>(request)), it->name);
				stream << "</li>";
			}
			stream << "</ul>";
		}

		bool ModuleAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks ModuleAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == HomeAdmin::FACTORY_KEY)
			{
				for (Factory<ModuleClass>::Iterator it = Factory<ModuleClass>::begin(); 
					it != Factory<ModuleClass>::end(); ++it)
				{
					AdminInterfaceElement::PageLink link;
					link.factoryKey = FACTORY_KEY;
					link.icon = ICON;
					link.parameterValue = it.getKey();
					link.parameterName = PARAMETER_MODULE;
					link.name = it->getName();

					if (!AdminInterfaceElement::GetAdminPage(link)->getSubPages(currentPage, request).empty())
						links.insert(links.begin(), link);
				}
			}
			return links;
		}

		std::string ModuleAdmin::getTitle() const
		{
			return _moduleClass.get() ? _moduleClass->getName() : DEFAULT_TITLE;
		}

		std::string ModuleAdmin::getParameterName() const
		{
			return _moduleClass.get() ? PARAMETER_MODULE : string();
		}

		std::string ModuleAdmin::getParameterValue() const
		{
			return _moduleClass.get() ? _moduleKey : string();
		}

		bool ModuleAdmin::isPageVisibleInTree(const AdminInterfaceElement& currentPage) const
		{
			return true;
		}		
	}
}
