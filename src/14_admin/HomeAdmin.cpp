
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
#include "UserAdmin.h"
#include "SecurityRight.h"
#include "HTMLModule.h"
#include "ServerModule.h"
#include "AdminFunction.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;
	using namespace security;
	using namespace html;	
	
	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, HomeAdmin>::FACTORY_KEY = "home";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<HomeAdmin>::ICON = "house.png";
		template<> const string AdminInterfaceElementTemplate<HomeAdmin>::DEFAULT_TITLE = "Accueil";



		void HomeAdmin::display(
			std::ostream& stream,
			const AdminRequest& request
		) const	{
			stream << "<p>Bienvenue sur le module d'administration de SYNTHESE " << ServerModule::VERSION << ".</p>";

			stream << "<h1>Accès directs</h1>";

			stream << "<ul>";

			BOOST_FOREACH(boost::shared_ptr<const AdminInterfaceElement> link, getSubPages(*this, request))
			{
				AdminRequest r(request, true);
				r.getFunction()->setPage(const_pointer_cast<AdminInterfaceElement>(link));
				
				stream << "<li>" << HTMLModule::getHTMLLink(r.getURL(), link->getTitle()) << "</li>";
			}

			stream << "</ul>";
		}



		void HomeAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}
		
		
		
		server::ParametersMap HomeAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		bool HomeAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}



		HomeAdmin::HomeAdmin()
			: AdminInterfaceElementTemplate<HomeAdmin>()
		{}



		AdminInterfaceElement::PageLinks HomeAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const {
			AdminInterfaceElement::PageLinks links;
			
			vector<shared_ptr<ModuleClass> > modules(
				Factory<ModuleClass>::GetNewCollection()
			);
			for(vector<shared_ptr<ModuleClass> >::const_reverse_iterator it(modules.rbegin()); it != modules.rend(); ++it)
			{
				shared_ptr<ModuleAdmin> link(
					getNewPage<ModuleAdmin>()
				);
				link->setModuleClass(
					const_pointer_cast<const ModuleClass, ModuleClass>(*it)
				);
				if (!link->getSubPages(currentPage, request).empty())
				{
					links.push_back(link);
				}
			}

			if(request.isAuthorized<SecurityRight>(UNKNOWN_RIGHT_LEVEL, READ, string()))
			{
				shared_ptr<UserAdmin> userPage(getNewPage<UserAdmin>());
				userPage->setUserC(request.getUser());
				links.push_back(userPage);
			}
			
			return links;
		}
}	}
