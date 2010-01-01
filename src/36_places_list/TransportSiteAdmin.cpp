
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
#include "PlacesListModule.h"
#include "SiteTableSync.h"
#include "Site.h"
#include "SiteUpdateAction.h"
#include "SiteRoutePlanningAdmin.h"
#include "TransportWebsiteRight.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "RoutePlannerFunction.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "ResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "HTMLModule.h"
#include "Interface.h"
#include "InterfaceTableSync.h"
#include "RoutePlannerInterfacePage.h"
#include "Profile.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace transportwebsite;
	using namespace html;
	using namespace routeplanner;
	using namespace security;

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
		
		void TransportSiteAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_site = SiteTableSync::GetEditable(map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY), _getEnv(), UP_LINKS_LOAD_LEVEL);
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}
		}
		
		
		
		server::ParametersMap TransportSiteAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_site.get())
				m.insert(Request::PARAMETER_OBJECT_ID, _site->getKey());
			return m;
		}


		
		void TransportSiteAdmin::display(ostream& stream, VariablesMap& variables,
					const admin::AdminRequest& _request) const
		{
			// Requests
			AdminActionFunctionRequest<SiteUpdateAction,TransportSiteAdmin> updateRequest(
				_request
			);
			updateRequest.getAction()->setSiteId(_site->getKey());

			AdminFunctionRequest<SiteRoutePlanningAdmin> routeplannerRequest(_request);
			routeplannerRequest.getPage()->setSite(_site);

			FunctionRequest<RoutePlannerFunction> rpHomeRequest(_request);
			rpHomeRequest.getFunction()->setSite(_site);

			// Display
			stream << "<h1>Liens</h1>";
			stream << "<p>";
			stream << HTMLModule::getLinkButton(routeplannerRequest.getURL(), "Calcul d'itinéraires (admin)", string(), AdminInterfaceElementTemplate<transportwebsite::SiteRoutePlanningAdmin>::ICON);
			stream << HTMLModule::getLinkButton(rpHomeRequest.getURL(), "Calcul d'itinéraires (home client)", string(), AdminInterfaceElementTemplate<transportwebsite::SiteRoutePlanningAdmin>::ICON);
			stream << "</p>";

			stream << "<h1>Propriétés</h1>";
			PropertiesHTMLTable pt(updateRequest.getHTMLForm());
			stream << pt.open();
			stream << pt.title("Identification");
			stream << pt.cell("Nom", pt.getForm().getTextInput(SiteUpdateAction::PARAMETER_NAME, _site->getName()));
			stream << pt.cell("Début validité", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_START_DATE, _site->getStartDate().toGregorianDate()));
			stream << pt.cell("Fin validité", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_END_DATE, _site->getEndDate().toGregorianDate()));
			stream << pt.title("Apparence");
			stream << pt.cell("Interface", pt.getForm().getSelectInput(
						SiteUpdateAction::PARAMETER_INTERFACE_ID,
						InterfaceTableSync::GetInterfaceLabels<RoutePlannerInterfacePage>(optional<string>()),
						_site->getInterface() ? _site->getInterface()->getKey() : 0
				)	);
			stream << pt.title("Recherche d'itinéraires");
			stream << pt.cell("Max correspondances", pt.getForm().getSelectNumberInput(SiteUpdateAction::PARAMETER_MAX_CONNECTIONS, 0, 99, _site->getMaxTransportConnectionsCount(), 1, "illimité"));
			stream << pt.cell("Réservation en ligne", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_ONLINE_BOOKING, _site->getOnlineBookingAllowed()));
			stream << pt.cell("Affichage données passées", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_USE_OLD_DATA, _site->getPastSolutionsDisplayed()));
			stream << pt.cell("Nombre de jours chargés", pt.getForm().getSelectNumberInput(SiteUpdateAction::PARAMETER_USE_DATES_RANGE, 0, 365, _site->getUseDatesRange(), 1, "illimité"));
			stream << pt.close();

			stream << "<h1>Périodes de recherche d'itinéraire</h1>";
			HTMLTable::ColsVector cv;
			cv.push_back("Nom");
			cv.push_back("Heure début");
			cv.push_back("Heure fin");
			HTMLTable ct(cv, ResultHTMLTable::CSS_CLASS);
			stream << ct.open();
			const Site::Periods& periods(_site->getPeriods());
			for (Site::Periods::const_iterator it(periods.begin()); it != periods.end(); ++it)
			{
				stream << ct.row();
				stream << ct.col() << it->getCaption();
				stream << ct.col() << it->getBeginHour().toString();
				stream << ct.col() << it->getEndHour().toString();
			}
			stream << ct.close();
			
			stream << "<h1>Périmètre de la base transport</h1>";
		}

		bool TransportSiteAdmin::isAuthorized(
			const security::Profile& profile
		) const	{
			return profile.isAuthorized<TransportWebsiteRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPagesOfModule(
			const string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(moduleKey == PlacesListModule::FACTORY_KEY && request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()->getProfile()))
			{
				SiteTableSync::SearchResult sites(
					SiteTableSync::Search(*_env)
				);
				BOOST_FOREACH(shared_ptr<Site> site, sites)
				{
					shared_ptr<TransportSiteAdmin> p(
						getNewOtherPage<TransportSiteAdmin>(false)
					);
					p->_site = site;
					links.push_back(p);
				}
			}
			return links;
		}
		

		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			shared_ptr<SiteRoutePlanningAdmin> p(
				getNewOtherPage<SiteRoutePlanningAdmin>()
			);
			p->setSite(_site);
			links.push_back(p);
			
			return links;
		}

		std::string TransportSiteAdmin::getTitle() const
		{
			return _site.get() ? _site->getName() : DEFAULT_TITLE;
		}
		
		
		bool TransportSiteAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _site == static_cast<const TransportSiteAdmin&>(other)._site;
		}
			
	}
}
