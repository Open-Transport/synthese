
/** HomeAdmin class implementation.
	@file HomeAdmin.cpp

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

#include "HomeAdmin.h"

#include "BaseAdminFunctionRequest.hpp"
#include "HTMLList.h"
#include "HTMLModule.h"
#include "ModuleAdmin.h"
#include "ModuleClass.h"
#include "Profile.h"
#include "SecurityRight.h"
#include "ServerModule.h"
#include "User.h"
#include "UserAdmin.h"

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
			const server::Request& request
		) const	{
			stream << "<p>Bienvenue sur le module d'administration de SYNTHESE " << ServerModule::VERSION << ".</p>";

			stream << "<h1>Accès directs</h1>";

			HTMLList l;
			stream << l.open();

			BOOST_FOREACH(const boost::shared_ptr<const AdminInterfaceElement>& link, getSubPages(*this, request))
			{
				BaseAdminFunctionRequest r(request);
				r.setPage(const_pointer_cast<AdminInterfaceElement>(link));

				stream << l.element() << HTMLModule::getHTMLLink(r.getURL(), link->getTitle());
			}

			stream << l.close();
		}



		void HomeAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}



		util::ParametersMap HomeAdmin::getParametersMap() const
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
			const server::Request& request
		) const {
			AdminInterfaceElement::PageLinks links;

			vector<boost::shared_ptr<ModuleClass> > modules(
				Factory<ModuleClass>::GetNewCollection()
			);
			for(vector<boost::shared_ptr<ModuleClass> >::const_reverse_iterator it(modules.rbegin()); it != modules.rend(); ++it)
			{
				boost::shared_ptr<ModuleAdmin> link(
					getNewPage<ModuleAdmin>()
				);
				link->setModuleClass(*it);
				if (!link->getSubPages(currentPage, request).empty())
				{
					links.push_back(link);
				}
			}

			if(request.getUser()->getProfile()->isAuthorized<SecurityRight>(UNKNOWN_RIGHT_LEVEL, READ, string()))
			{
				boost::shared_ptr<UserAdmin> userPage(getNewPage<UserAdmin>());
				userPage->setUserC(request.getUser());
				links.push_back(userPage);
			}

			return links;
		}
}	}
