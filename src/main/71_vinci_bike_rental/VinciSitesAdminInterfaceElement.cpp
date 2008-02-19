
/** VinciSitesAdminInterfaceElement class implementation.
	@file VinciSitesAdminInterfaceElement.cpp
	@author Hugues Romain
	@date 2007

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

#include "71_vinci_bike_rental/VinciSitesAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciRight.h"
#include "71_vinci_bike_rental/VinciSiteAddAction.h"
#include "71_vinci_bike_rental/VinciSiteAdmin.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminRequest.h"

#include "30_server/ActionFunctionRequest.h"

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/SearchFormHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace vinci;
	using namespace security;
	using namespace html;

	namespace util
	{
	    template <> const string FactorableTemplate<AdminInterfaceElement,VinciSitesAdminInterfaceElement>::FACTORY_KEY("vincisites");
	}

	namespace admin
	{
		template <> const string AdminInterfaceElementTemplate<VinciSitesAdminInterfaceElement>::ICON("building.png");
		template <> const string AdminInterfaceElementTemplate<VinciSitesAdminInterfaceElement>::DEFAULT_TITLE("Sites");
	}

	namespace vinci
	{
		const std::string VinciSitesAdminInterfaceElement::PARAMETER_NAME("na");
		const std::string VinciSitesAdminInterfaceElement::PARAMETER_SITE("si");
		const std::string VinciSitesAdminInterfaceElement::PARAMETER_ALERTS("al");
		const std::string VinciSitesAdminInterfaceElement::PARAMETER_LOCK("lo");


		VinciSitesAdminInterfaceElement::VinciSitesAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciSitesAdminInterfaceElement>() {}

		void VinciSitesAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			_searchName = map.getString(PARAMETER_NAME, false, FACTORY_KEY);
			_requestParameters = ResultHTMLTable::getParameters(map.getMap(), string(), 30);
		}


		void VinciSitesAdminInterfaceElement::display( std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request /*= NULL*/ ) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<VinciSitesAdminInterfaceElement>();

			ActionFunctionRequest<VinciSiteAddAction,AdminRequest> addRequest(request);
			addRequest.getFunction()->setPage<VinciSiteAdmin>();
			addRequest.getFunction()->setActionFailedPage<VinciSitesAdminInterfaceElement>();

			FunctionRequest<AdminRequest> goRequest(request);
			goRequest.getFunction()->setPage<VinciSiteAdmin>();

			// Search
			vector<shared_ptr<VinciSite> > sites(
				VinciSiteTableSync::search(
					"%"+_searchName+"%"
					, UNKNOWN_VALUE
					, _requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_NAME
					, _requestParameters.raisingOrder
			)	);

			// HTML search form
			stream << "<h1>Recherche</h1>";
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAMETER_NAME, _searchName));
			stream << st.close();

			// Results list
			stream << "<h1>Sites</h1>";

			ResultHTMLTable::ResultParameters p(ResultHTMLTable::getParameters(_requestParameters, sites));

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_NAME, "Nom"));
			h.push_back(make_pair(PARAMETER_SITE, "Site d'alimentation"));
			h.push_back(make_pair(PARAMETER_ALERTS, "Alerte Stock"));
			h.push_back(make_pair(string(), "Edition"));
			h.push_back(make_pair(PARAMETER_LOCK, "Liste"));

			ResultHTMLTable t(h, st.getForm(), _requestParameters, p);
			stream << t.open();

			for (vector<shared_ptr<VinciSite> >::const_iterator it(sites.begin()); it != sites.end(); ++it)
			{
				goRequest.setObjectId((*it)->getKey());

				
				stream << t.row();
				stream << t.col() << (*it)->getName();
				stream << t.col();
				if ((*it)->getParentSiteId() > 0)
				{
					shared_ptr<const VinciSite> parentSite(VinciSiteTableSync::Get((*it)->getParentSiteId()));
					stream << parentSite->getName();
				}
				else
					stream << "AUCUN";

				stream << t.col();
				stream << t.col() << HTMLModule::getLinkButton(goRequest.getURL(), "Ouvrir", string(), "building_edit.png");
				stream << t.col() << ((*it)->getLocked() ? "NON" : "OUI");
			}

			HTMLForm af(addRequest.getHTMLForm("add"));

			stream << t.row();
			stream << t.col() << af.open();
			stream << af.getTextInput(VinciSiteAddAction::PARAMETER_NAME, string(), "(entrer le nom du site ici)");
			stream << af.getSubmitButton("Ajouter");
			stream << af.close();

			stream << t.close();
		}

		bool VinciSitesAdminInterfaceElement::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<VinciRight>(READ);
		}

		AdminInterfaceElement::PageLinks VinciSitesAdminInterfaceElement::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.factoryKey == VinciBikeRentalModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}
	}
}
