
/** TransportSiteAdmin class implementation.
	@file TransportSiteAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "WebsiteAdmin.hpp"

#include "AlgorithmLogger.hpp"
#include "WebsiteTableSync.hpp"
#include "Website.hpp"
#include "SiteUpdateAction.h"
#include "RoutePlannerFunction.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "ResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "HTMLModule.h"
#include "Interface.h"
#include "InterfaceTableSync.h"
#include "StaticFunctionRequest.h"
#include "Profile.h"
#include "JourneyPattern.hpp"
#include "PTTimeSlotRoutePlanner.h"
#include "SearchFormHTMLTable.h"
#include "HTMLForm.h"
#include "PTRoutePlannerResult.h"
#include "WebPageAdmin.h"
#include "ActionResultHTMLTable.h"
#include "WebPageAddAction.h"
#include "AdminFunctionRequest.hpp"
#include "WebPageDisplayFunction.h"
#include "City.h"
#include "CityTableSync.h"
#include "PTPlacesAdmin.h"
#include "ObjectSiteLink.h"
#include "ObjectSiteLinkTableSync.h"
#include "WebPageMoveAction.hpp"
#include "SiteCityAddAction.hpp"
#include "RoadJourneyPlanner.h"
#include "RoadModule.h"
#include "RemoveObjectAction.hpp"

#include <geos/geom/Point.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace pt_website;
	using namespace html;
	using namespace security;
	using namespace pt_journey_planner;
	using namespace algorithm;
	using namespace html;
	using namespace pt;
	using namespace graph;
	using namespace geography;
	using namespace road;
	using namespace cms;
	using namespace road_journey_planner;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TransportSiteAdmin>::FACTORY_KEY("TransportSiteAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TransportSiteAdmin>::ICON("layout.png");
		template<> const string AdminInterfaceElementTemplate<TransportSiteAdmin>::DEFAULT_TITLE("Site inconnu");
	}

	namespace pt_website
	{
		const string TransportSiteAdmin::PARAMETER_SEARCH_PAGE = "pp";
		const string TransportSiteAdmin::PARAMETER_SEARCH_RANK = "pr";
		const string TransportSiteAdmin::PARAMETER_JOURNEY_PLANNING_ALGORITHM = "ja";

		const string TransportSiteAdmin::TAB_PROPERTIES("pr");
		const string TransportSiteAdmin::TAB_PERIMETER("pe");
		const string TransportSiteAdmin::TAB_ROUTE_PLANNING("rp");
		const string TransportSiteAdmin::TAB_WEB_PAGES("wp");



		TransportSiteAdmin::TransportSiteAdmin():
			AdminInterfaceElementTemplate<TransportSiteAdmin>(),
			_pt_journey_planning(true)
		{}



		void TransportSiteAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Site
			try
			{
				_site = WebsiteTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}

			// Search page
			_searchPage = map.getDefault<string>(PARAMETER_SEARCH_PAGE);
			_pageSearchParameter.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_RANK, optional<size_t>());

			// Journey planner
			_journeyPlanner.setSite(_site.get());
			_journeyPlanner._setFromParametersMap(map);
			_journeyPlanner.setOutputFormat(RoutePlannerFunction::VALUE_ADMIN_HTML);

			// Use PT algorithm
			_pt_journey_planning = map.getDefault<bool>(PARAMETER_JOURNEY_PLANNING_ALGORITHM, true);
		}



		util::ParametersMap TransportSiteAdmin::getParametersMap() const
		{
			ParametersMap m(_pageSearchParameter.getParametersMap());

			m.merge(_journeyPlanner._getParametersMap());
			m.insert(PARAMETER_SEARCH_PAGE, _searchPage);
			m.insert(PARAMETER_JOURNEY_PLANNING_ALGORITHM, _pt_journey_planning);
			if(_site.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _site->getKey());
			}
			return m;
		}



		void TransportSiteAdmin::display(
			ostream& stream,
			const admin::AdminRequest& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{

				// Requests
				AdminActionFunctionRequest<SiteUpdateAction,TransportSiteAdmin> updateRequest(
					_request
				);
				updateRequest.getAction()->setSiteId(_site->getKey());

				StaticFunctionRequest<RoutePlannerFunction> rpHomeRequest(_request, true);
				rpHomeRequest.getFunction()->setSite(_site.get());

				// Display
				stream << "<h1>Liens</h1>";
				stream << "<p>";
				stream << HTMLModule::getLinkButton(rpHomeRequest.getURL(), "Calcul d'itin�raires (home client)", string(), "arrow_switch.png");
				stream << "</p>";

				stream << "<h1>Propri�t�s</h1>";
				PropertiesHTMLTable pt(updateRequest.getHTMLForm());
				stream << pt.open();
				stream << pt.title("Identification");
				stream << pt.cell("Nom", pt.getForm().getTextInput(SiteUpdateAction::PARAMETER_NAME, _site->get<Name>()));
				stream << pt.cell("D�but validit�", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_START_DATE, _site->getStartDate()));
				stream << pt.cell("Fin validit�", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_END_DATE, _site->getEndDate()));
				stream << pt.cell("URL", pt.getForm().getTextInput(SiteUpdateAction::PARAMETER_CLIENT_URL, _site->getClientURL()));
				stream << pt.cell(
					"Mod�le de page par d�faut",
					pt.getForm().getTextInput(
						SiteUpdateAction::PARAMETER_DEFAULT_PAGE_TEMPLATE_ID,
						lexical_cast<string>(_site->getDefaultTemplate() ? _site->getDefaultTemplate()->getKey() : RegistryKeyType(0))
				)	);
				stream << pt.title("Recherche d'itin�raires");
				stream << pt.cell("Max correspondances", pt.getForm().getSelectNumberInput(SiteUpdateAction::PARAMETER_MAX_CONNECTIONS, 0, 99, _site->getMaxTransportConnectionsCount(), 1, "illimit�"));
				stream << pt.cell("R�servation en ligne", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_ONLINE_BOOKING, _site->getOnlineBookingAllowed()));
				stream << pt.cell("Affichage donn�es pass�es", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_USE_OLD_DATA, _site->getPastSolutionsDisplayed()));
				stream << pt.cell("Nombre de jours charg�s", pt.getForm().getSelectNumberInput(SiteUpdateAction::PARAMETER_USE_DATES_RANGE, 0, 365, _site->getUseDatesRange().days(), 1, "illimit�"));
				stream << pt.cell("Affichage d�tail approche routi�re", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL, _site->getDisplayRoadApproachDetail()));
				stream << pt.close();

				stream << "<h1>P�riodes de recherche d'itin�raire</h1>";
				HTMLTable::ColsVector cv;
				cv.push_back("Nom");
				cv.push_back("Heure d�but");
				cv.push_back("Heure fin");
				HTMLTable ct(cv, ResultHTMLTable::CSS_CLASS);
				stream << ct.open();
				const Website::Periods& periods(_site->getPeriods());
				for (Website::Periods::const_iterator it(periods.begin()); it != periods.end(); ++it)
				{
					stream << ct.row();
					stream << ct.col() << it->getCaption();
					stream << ct.col() << it->getBeginHour();
					stream << ct.col() << it->getEndHour();
				}
				stream << ct.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB PERIMETER
			if (openTabContent(stream, TAB_PERIMETER))
			{
				stream << "<h1>Communes autoris�es</h1>";

				ObjectSiteLinkTableSync::SearchResult cities(
					ObjectSiteLinkTableSync::Search(
						_getEnv(),
						_site->getKey(),
						optional<RegistryKeyType>(),
						CityTableSync::TABLE.ID
				)	);

				AdminFunctionRequest<PTPlacesAdmin> openCityRequest(_request);

				AdminActionFunctionRequest<SiteCityAddAction,TransportSiteAdmin> cityAddRequest(_request);
				cityAddRequest.getAction()->setSite(_site);

				AdminActionFunctionRequest<RemoveObjectAction,TransportSiteAdmin> cityRemoveRequest(_request);

				HTMLForm f(cityAddRequest.getHTMLForm("add_city"));
				HTMLTable::ColsVector v;
				v.push_back("Localit�");
				v.push_back("Actions");
				v.push_back("Actions");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
				stream << f.open() << t.open();
				BOOST_FOREACH(const shared_ptr<ObjectSiteLink>& link, cities)
				{
					cityRemoveRequest.getAction()->setObjectId(link->getKey());

					stream << t.row();
					if(Env::GetOfficialEnv().getRegistry<City>().contains(link->getObjectId()))
					{
						shared_ptr<const City> city(Env::GetOfficialEnv().get<City>(link->getObjectId()));

						openCityRequest.getPage()->setCity(city);

						stream << t.col() << city->getName();
						stream << t.col() << HTMLModule::getLinkButton(openCityRequest.getURL(), "Ouvrir", string(), PTPlacesAdmin::ICON);
					}
					else
					{
						stream << t.col() << "Unconsistent city " << link->getObjectId();
						stream << t.col();
					}
					stream << t.col() << HTMLModule::getLinkButton(cityRemoveRequest.getHTMLForm().getURL(), "Supprimer", "Etes-vous s�e de vouloir supprimer le lien ?");
				}
				stream << t.row();
				stream << t.col(2) << f.getTextInput(SiteCityAddAction::PARAMETER_CITY_NAME, string());
				stream << t.col() << f.getSubmitButton("Ajouter");

				stream << t.close() << f.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB WEB PAGES
			if (openTabContent(stream, TAB_WEB_PAGES))
			{
				stream << "<h1>Pages</h1>";
				AdminActionFunctionRequest<WebPageAddAction, TransportSiteAdmin> addRequest(_request);
				addRequest.getAction()->setSite(const_pointer_cast<Website>(_site));

				AdminActionFunctionRequest<RemoveObjectAction, TransportSiteAdmin> deleteRequest(_request);

				AdminActionFunctionRequest<WebPageMoveAction, TransportSiteAdmin> moveRequest(_request);

				WebPageAdmin::DisplaySubPages(stream, _site->getKey(), addRequest, deleteRequest, moveRequest, _request);
			}


			////////////////////////////////////////////////////////////////////
			// TAB ROUTE PLANNER
			if (openTabContent(stream, TAB_ROUTE_PLANNING))
			{
				AdminFunctionRequest<TransportSiteAdmin> searchRequest(_request);

				// Search form
				stream << "<h1>Recherche</h1>";

				SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
				stream << st.open();
				stream << st.cell("Commune d�part", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT, _journeyPlanner.getDeparturePlace().cityResult.key.getSource()));
				stream << st.cell("Arr�t d�part", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT, dynamic_cast<NamedPlace*>(_journeyPlanner.getDeparturePlace().placeResult.value.get()) ? _journeyPlanner.getDeparturePlace().placeResult.key.getSource() : string()));
				stream << st.cell("Commune arriv�e", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT, _journeyPlanner.getArrivalPlace().cityResult.key.getSource()));
				stream << st.cell("Arr�t arriv�e", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT, dynamic_cast<NamedPlace*>(_journeyPlanner.getArrivalPlace().placeResult.value.get()) ? _journeyPlanner.getArrivalPlace().placeResult.key.getSource() : string()));
				stream << st.cell("Date/Heure", st.getForm().getCalendarInput(RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME, _journeyPlanner.getStartDepartureDate()));
				stream << st.cell(
					"Nombre r�ponses",
					st.getForm().getSelectNumberInput(
						RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER,
						1, 99,
						_journeyPlanner.getMaxSolutionsNumber() ? *_journeyPlanner.getMaxSolutionsNumber() : UNKNOWN_VALUE,
						1,
						"(illimit�)"
				)	);
				stream << st.cell(
					"Accessibilit�",
					st.getForm().getSelectInput(
						RoutePlannerFunction::PARAMETER_ACCESSIBILITY,
						WebsiteModule::GetAccessibilityNames(),
						optional<UserClassCode>(_journeyPlanner.getAccessParameters().getUserClass())
				)	);
				stream << st.cell(
					"Trace",
					st.getForm().getTextInput(
						RoutePlannerFunction::PARAMETER_LOG_PATH,
						_journeyPlanner.getLogger().getDirectory().file_string()
				)	);
				if(!_site->getRollingStockFilters().empty())
				{
					stream << st.cell(
						"Modes de transport",
						st.getForm().getSelectInput(
							RoutePlannerFunction::PARAMETER_ROLLING_STOCK_FILTER_ID,
							_site->getRollingStockFiltersList(),
							_journeyPlanner.getTransportModeFilter() ? optional<size_t>(_journeyPlanner.getTransportModeFilter()->getRank()) : optional<size_t>()
					)	);
				}
				stream << st.cell("Transport public", st.getForm().getOuiNonRadioInput(PARAMETER_JOURNEY_PLANNING_ALGORITHM, _pt_journey_planning));
				stream << st.cell(
					"Dur�e max correspondance",
					st.getForm().getSelectNumberInput(
						RoutePlannerFunction::PARAMETER_MAX_TRANSFER_DURATION,
						1, 99,
						_journeyPlanner.getMaxTransferDuration() ? (_journeyPlanner.getMaxTransferDuration()->total_seconds() / 60) : 60,
						1,
						"(illimit�)"
				)	);
				stream << st.cell(
					"Vitesse d'approche",
					st.getForm().getTextInput(
						RoutePlannerFunction::PARAMETER_APPROACH_SPEED,
						lexical_cast<string>(_journeyPlanner.getAccessParameters().getApproachSpeed())
				)	);
				stream << st.close();

				stream << "<h1>R�sultats</h1>";

				if(_pt_journey_planning)
				{
					_journeyPlanner.run(stream, _request);
				}
				else
				{
					// Route planning
					AccessParameters ap(
						USER_PEDESTRIAN,
						false, false, 30000, posix_time::hours(5), 1.111,
						1000
					);
					RoadJourneyPlanner r(
						_journeyPlanner.getDeparturePlace().placeResult.value.get(),
						_journeyPlanner.getArrivalPlace().placeResult.value.get(),
						_journeyPlanner.getStartDepartureDate(),
						_journeyPlanner.getStartDepartureDate(),
						_journeyPlanner.getStartDepartureDate(),
						_journeyPlanner.getStartDepartureDate() + hours(24),
						1,
						ap,
						DEPARTURE_FIRST,
						_journeyPlanner.getLogger()
					);
					RoadJourneyPlannerResult jv(r.run());

					jv.displayHTMLTable(stream);

					if(jv.getJourneys().size())
					{
						stream << "<h2>Carte</h2><div id=\"olmap\"></div>" << std::endl;
						stream << HTMLModule::GetHTMLJavascriptOpen();
						stream << "var tripWKT=\"" << jv.getTripWKT() << "\";";
						stream << HTMLModule::GetHTMLJavascriptClose();
						stream << HTMLModule::GetHTMLJavascriptOpen("http://proj4js.org/lib/proj4js-compressed.js");
						stream << HTMLModule::GetHTMLJavascriptOpen("http://www.openlayers.org/api/OpenLayers.js");
						stream << HTMLModule::GetHTMLJavascriptOpen("http://www.openstreetmap.org/openlayers/OpenStreetMap.js");
						stream << HTMLModule::GetHTMLJavascriptOpen("pedestrianroutemap.js");
					}
				}
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		bool TransportSiteAdmin::isAuthorized(
			const security::User& user
		) const	{
return true;
//			return user.getProfile()->isAuthorized<WebsiteRight>(READ);
		}



		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const WebsiteModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				WebsiteTableSync::SearchResult sites(
					WebsiteTableSync::Search(Env::GetOfficialEnv())
				);
				BOOST_FOREACH(const shared_ptr<Website>& site, sites)
				{
					shared_ptr<TransportSiteAdmin> p(
						getNewPage<TransportSiteAdmin>()
					);
					p->_site = const_pointer_cast<const Website>(site);
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

			if(	currentPage == *this ||
				dynamic_cast<const WebPageAdmin*>(&currentPage)
			){
				WebPageTableSync::SearchResult pages(WebPageTableSync::Search(Env::GetOfficialEnv(), _site->getKey(), RegistryKeyType(0)));
				BOOST_FOREACH(const shared_ptr<Webpage>& page, pages)
				{
					shared_ptr<WebPageAdmin> p(
						getNewPage<WebPageAdmin>()
					);
					p->setPage(const_pointer_cast<const Webpage>(page));
					links.push_back(p);
				}	}

			return links;
		}



		std::string TransportSiteAdmin::getTitle() const
		{
			return _site.get() ? _site->getName() : DEFAULT_TITLE;
		}




		bool TransportSiteAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _site->getKey() == static_cast<const TransportSiteAdmin&>(other)._site->getKey();
		}



		void TransportSiteAdmin::_buildTabs( const security::Profile& profile ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Propri�t�s", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("P�rim�tre base transport", TAB_PERIMETER, true));
			_tabs.push_back(Tab("Pages web", TAB_WEB_PAGES, true));
			_tabs.push_back(Tab("Calcul d'itin�raires", TAB_ROUTE_PLANNING, true));

			_tabBuilded = true;
		}
	}
}
