
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

#include "QueryString.h"
#include "ActionFunctionRequest.h"
#include "Request.h"

#include "RoutePlannerFunction.h"

#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminRequest.h"

#include "ResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "HTMLModule.h"

#include "Interface.h"
#include "InterfaceModule.h"

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
		
		void TransportSiteAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_site = SiteTableSync::GetEditable(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}
		}
		
		void TransportSiteAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			ActionFunctionRequest<SiteUpdateAction,AdminRequest> updateRequest(request);
			updateRequest.getAction()->setSiteId(_site->getKey());
			updateRequest.getFunction()->setPage<TransportSiteAdmin>();
			updateRequest.setObjectId(_site->getKey());

			FunctionRequest<AdminRequest> routeplannerRequest(request);
			routeplannerRequest.getFunction()->setPage<SiteRoutePlanningAdmin>();
			routeplannerRequest.setObjectId(_site->getKey());

			FunctionRequest<RoutePlannerFunction> rpHomeRequest(request);
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
			stream << pt.cell("Début validité", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_START_DATE, _site->getStartDate()));
			stream << pt.cell("Fin validité", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_END_DATE, _site->getEndDate()));
			stream << pt.title("Apparence");
			stream << pt.cell("Interface", pt.getForm().getSelectInput(SiteUpdateAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(), _site->getInterface()->getKey()));
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

		bool TransportSiteAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<TransportWebsiteRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == PlacesListModule::FACTORY_KEY)
			{
				Env env;
				SiteTableSync::Search(env);
				BOOST_FOREACH(shared_ptr<Site> site, env.template getRegistry<Site>())
				{
					PageLink link;
					link.factoryKey = FACTORY_KEY;
					link.icon = ICON;
					link.name = site->getName();
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(site->getKey());
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
