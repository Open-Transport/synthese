
/** HomeAdmin class implementation.
	@file HomeAdmin.cpp

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

#include "HomeAdmin.h"
#include "ModuleAdmin.h"
#include "ModuleClass.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, HomeAdmin>::FACTORY_KEY = "home";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<HomeAdmin>::ICON = "house.png";
		template<> const string AdminInterfaceElementTemplate<HomeAdmin>::DEFAULT_TITLE = "Accueil";

		void HomeAdmin::display( std::ostream& stream, interfaces::VariablesMap& variables,
					const FunctionRequest<admin::AdminRequest>& request) const
		{
			stream << "Bienvenue sur le panneau de configuration de SYNTHESE";
		}

		void HomeAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
		}
		
		
		
		server::ParametersMap HomeAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		bool HomeAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			return true;
		}

		HomeAdmin::HomeAdmin()
			: AdminInterfaceElementTemplate<HomeAdmin>()
		{
	
		}


		AdminInterfaceElement::PageLinks HomeAdmin::getSubPages(
			boost::shared_ptr<const AdminInterfaceElement> currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const {
			AdminInterfaceElement::PageLinks links;
			
			const ModuleAdmin* ma(
				dynamic_cast<const ModuleAdmin*>(currentPage.get())
			);
			const string moduleKey(
				ma ?
				ma->getModuleClass()->getFactoryKey() :
				string()
			);

			vector<shared_ptr<ModuleClass> > modules(
				Factory<ModuleClass>::GetNewCollection()
			);
			BOOST_FOREACH(shared_ptr<ModuleClass> module, modules)
			{
				if(	ma &&
					moduleKey == module->getFactoryKey()
				){
					AddToLinks(links, currentPage);
				}
				else
				{
					shared_ptr<ModuleAdmin> link(
						getNewOtherPage<ModuleAdmin>()
					);
					link->setModuleClass(
						const_pointer_cast<const ModuleClass, ModuleClass>(module)
					);
					if (!link->getSubPages(currentPage, request).empty())
					{
						AddToLinks(links, link);
					}
				}
			}
			
			return links;
		}
	}
}
