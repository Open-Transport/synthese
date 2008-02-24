
/** TransportSiteAdmin class implementation.
	@file TransportSiteAdmin.cpp
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

#include "TransportSiteAdmin.h"
#include "36_places_list/PlacesListModule.h"
#include "36_places_list/SiteTableSync.h"
#include "36_places_list/Site.h"

#include "30_server/QueryString.h"

#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminParametersException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace transportwebsite;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TransportSiteAdmin>::FACTORY_KEY("TransportSiteAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TransportSiteAdmin>::ICON("layout.png");
		template<> const string AdminInterfaceElementTemplate<TransportSiteAdmin>::DEFAULT_TITLE("Site inconnu");
	}

	namespace transportwebsite
	{
		TransportSiteAdmin::TransportSiteAdmin()
			: AdminInterfaceElementTemplate<TransportSiteAdmin>()
		{ }
		
		void TransportSiteAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_site = SiteTableSync::GetUpdateable(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}
		}
		
		void TransportSiteAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			stream << "Not yet implemented, use the tree to navigate.";
		}

		bool TransportSiteAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == PlacesListModule::FACTORY_KEY)
			{
				vector<shared_ptr<Site> > sites(SiteTableSync::search());
				for (vector<shared_ptr<Site> >::const_iterator it(sites.begin()); it != sites.end(); ++it)
				{
					PageLink link;
					link.factoryKey = FACTORY_KEY;
					link.icon = ICON;
					link.name = (*it)->getName();
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString((*it)->getKey());
					links.push_back(link);
				}
			}
			return links;
		}
		

		std::string TransportSiteAdmin::getTitle() const
		{
			return _site.get() ? _site->getName() : DEFAULT_TITLE;
		}

		std::string TransportSiteAdmin::getParameterName() const
		{
			return _site.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string TransportSiteAdmin::getParameterValue() const
		{
			return _site.get() ? Conversion::ToString(_site->getKey()) : string();
		}
	}
}
