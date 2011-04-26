
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
#include "TransportWebsiteModule.h"
#include "TransportWebsiteTableSync.h"
#include "TransportWebsite.h"
#include "SiteUpdateAction.h"
#include "TransportWebsiteRight.h"
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
		const string TransportSiteAdmin::PARAMETER_SEARCH_PAGE("pp");
		const string TransportSiteAdmin::PARAMETER_SEARCH_RANK("pr");
		const string TransportSiteAdmin::PARAMETER_DATE_TIME("dt");
		const string TransportSiteAdmin::PARAMETER_START_CITY("sc");
		const string TransportSiteAdmin::PARAMETER_START_PLACE("sp");
		const string TransportSiteAdmin::PARAMETER_END_CITY("ec");
		const string TransportSiteAdmin::PARAMETER_END_PLACE("ep");
		const string TransportSiteAdmin::PARAMETER_RESULTS_NUMBER("rn");
		const string TransportSiteAdmin::PARAMETER_ACCESSIBILITY("ac");
		const string TransportSiteAdmin::PARAMETER_LOG("lo");
		const string TransportSiteAdmin::PARAMETER_ROLLING_STOCK_FILTER("rf");
		const string TransportSiteAdmin::PARAMETER_JOURNEY_PLANNING_ALGORITHM("ja");

		const string TransportSiteAdmin::TAB_PROPERTIES("pr");
		const string TransportSiteAdmin::TAB_PERIMETER("pe");
		const string TransportSiteAdmin::TAB_ROUTE_PLANNING("rp");
		const string TransportSiteAdmin::TAB_WEB_PAGES("wp");
		
		TransportSiteAdmin::TransportSiteAdmin()
			: AdminInterfaceElementTemplate<TransportSiteAdmin>(),
			_dateTime(not_a_date_time),
			_accessibility(USER_PEDESTRIAN),
			_log(false),
			_rollingStockFilter(NULL),
			_pt_journey_planning(true)
		{ }
		
		void TransportSiteAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_site = TransportWebsiteTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}

			_searchPage = map.getDefault<string>(PARAMETER_SEARCH_PAGE);
			_pageSearchParameter.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_RANK, optional<size_t>());

			_startCity = map.getDefault<string>(PARAMETER_START_CITY);
			_endCity = map.getDefault<string>(PARAMETER_END_CITY);
			_startPlace = map.getDefault<string>(PARAMETER_START_PLACE);
			_endPlace = map.getDefault<string>(PARAMETER_END_PLACE);
			_log = map.getDefault<bool>(PARAMETER_LOG, false);
			_pageSearchParameter.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_RANK);
			_pt_journey_planning = map.getDefault<bool>(PARAMETER_JOURNEY_PLANNING_ALGORITHM, true);

			if(!map.getDefault<string>(PARAMETER_DATE_TIME).empty())
			{
				_dateTime = time_from_string(map.get<string>(PARAMETER_DATE_TIME));
			}
			else
			{
				_dateTime = ptime(second_clock::local_time());
			}
			_resultsNumber = map.getOptional<size_t>(PARAMETER_RESULTS_NUMBER);
			_accessibility = static_cast<UserClassCode>(
				map.getDefault<int>(PARAMETER_ACCESSIBILITY, UNKNOWN_VALUE)
			);
		
			if(!_site->getRollingStockFilters().empty())
			{
				if(map.getOptional<size_t>(PARAMETER_ROLLING_STOCK_FILTER))
				{
					TransportWebsite::RollingStockFilters::const_iterator it(_site->getRollingStockFilters().find(map.get<size_t>(PARAMETER_ROLLING_STOCK_FILTER)));
					if(it == _site->getRollingStockFilters().end())
					{
						throw AdminParametersException("No such rolling stock filter");
					}
					_rollingStockFilter = it->second;
				}
				else
				{
					_rollingStockFilter = _site->getRollingStockFilters().begin()->second;
				}
			}
		}
		
		
		
		server::ParametersMap TransportSiteAdmin::getParametersMap() const
		{
			ParametersMap m(_pageSearchParameter.getParametersMap());
			m.insert(PARAMETER_START_CITY, _startCity);
			m.insert(PARAMETER_START_PLACE, _startPlace);
			m.insert(PARAMETER_END_CITY, _endCity);
			m.insert(PARAMETER_END_PLACE, _endPlace);
			if(!_dateTime.is_not_a_date_time())
			{
				m.insert(PARAMETER_DATE_TIME, _dateTime);
			}
			m.insert(PARAMETER_LOG, _log);
			m.insert(PARAMETER_RESULTS_NUMBER, _resultsNumber);
			m.insert(PARAMETER_ACCESSIBILITY, static_cast<int>(_accessibility));
			m.insert(PARAMETER_SEARCH_PAGE, _searchPage);
			m.insert(PARAMETER_JOURNEY_PLANNING_ALGORITHM, _pt_journey_planning);
			if(_site.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _site->getKey());
				if(_rollingStockFilter)
				{
					m.insert(PARAMETER_ROLLING_STOCK_FILTER, static_cast<int>(_rollingStockFilter->getRank()));
				}
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
				rpHomeRequest.getFunction()->setSite(_site);

				// Display
				stream << "<h1>Liens</h1>";
				stream << "<p>";
				stream << HTMLModule::getLinkButton(rpHomeRequest.getURL(), "Calcul d'itinéraires (home client)", string(), "arrow_switch.png");
				stream << "</p>";

				stream << "<h1>Propriétés</h1>";
				PropertiesHTMLTable pt(updateRequest.getHTMLForm());
				stream << pt.open();
				stream << pt.title("Identification");
				stream << pt.cell("Nom", pt.getForm().getTextInput(SiteUpdateAction::PARAMETER_NAME, _site->getName()));
				stream << pt.cell("Début validité", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_START_DATE, _site->getStartDate()));
				stream << pt.cell("Fin validité", pt.getForm().getCalendarInput(SiteUpdateAction::PARAMETER_END_DATE, _site->getEndDate()));
				stream << pt.cell("URL", pt.getForm().getTextInput(SiteUpdateAction::PARAMETER_CLIENT_URL, _site->getClientURL()));
				stream << pt.cell(
					"Modèle de page par défaut",
					pt.getForm().getTextInput(
						SiteUpdateAction::PARAMETER_DEFAULT_PAGE_TEMPLATE_ID,
						lexical_cast<string>(_site->getDefaultTemplate() ? _site->getDefaultTemplate()->getKey() : RegistryKeyType(0))
				)	);
				stream << pt.title("Recherche d'itinéraires");
				stream << pt.cell("Max correspondances", pt.getForm().getSelectNumberInput(SiteUpdateAction::PARAMETER_MAX_CONNECTIONS, 0, 99, _site->getMaxTransportConnectionsCount(), 1, "illimité"));
				stream << pt.cell("Réservation en ligne", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_ONLINE_BOOKING, _site->getOnlineBookingAllowed()));
				stream << pt.cell("Affichage données passées", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_USE_OLD_DATA, _site->getPastSolutionsDisplayed()));
				stream << pt.cell("Nombre de jours chargés", pt.getForm().getSelectNumberInput(SiteUpdateAction::PARAMETER_USE_DATES_RANGE, 0, 365, _site->getUseDatesRange().days(), 1, "illimité"));
				stream << pt.cell("Affichage détail approche routière", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL, _site->getDisplayRoadApproachDetail()));
				stream << pt.close();

				stream << "<h1>Périodes de recherche d'itinéraire</h1>";
				HTMLTable::ColsVector cv;
				cv.push_back("Nom");
				cv.push_back("Heure début");
				cv.push_back("Heure fin");
				HTMLTable ct(cv, ResultHTMLTable::CSS_CLASS);
				stream << ct.open();
				const TransportWebsite::Periods& periods(_site->getPeriods());
				for (TransportWebsite::Periods::const_iterator it(periods.begin()); it != periods.end(); ++it)
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
				stream << "<h1>Communes autorisées</h1>";

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
				v.push_back("Localité");
				v.push_back("Actions");
				v.push_back("Actions");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
				stream << f.open() << t.open();
				BOOST_FOREACH(shared_ptr<ObjectSiteLink> link, cities)
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
					stream << t.col() << HTMLModule::getLinkButton(cityRemoveRequest.getHTMLForm().getURL(), "Supprimer", "Etes-vous sûe de vouloir supprimer le lien ?");
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
				addRequest.getAction()->setSite(const_pointer_cast<TransportWebsite>(_site));

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
				stream << st.cell("Commune départ", st.getForm().getTextInput(PARAMETER_START_CITY, _startCity));
				stream << st.cell("Arrêt départ", st.getForm().getTextInput(PARAMETER_START_PLACE, _startPlace));
				stream << st.cell("Commune arrivée", st.getForm().getTextInput(PARAMETER_END_CITY, _endCity));
				stream << st.cell("Arrêt arrivée", st.getForm().getTextInput(PARAMETER_END_PLACE, _endPlace));
				stream << st.cell("Date/Heure", st.getForm().getCalendarInput(PARAMETER_DATE_TIME, _dateTime));
				stream << st.cell(
					"Nombre réponses",
					st.getForm().getSelectNumberInput(
						PARAMETER_RESULTS_NUMBER,
						1, 99,
						_resultsNumber ? *_resultsNumber : UNKNOWN_VALUE,
						1,
						"(illimité)"
				)	);
				stream << st.cell(
					"Accessibilité",
					st.getForm().getSelectInput(PARAMETER_ACCESSIBILITY, TransportWebsiteModule::GetAccessibilityNames(), optional<UserClassCode>(_accessibility)));
				stream << st.cell("Trace", st.getForm().getOuiNonRadioInput(PARAMETER_LOG, _log));
				if(!_site->getRollingStockFilters().empty())
				{
					stream << st.cell(
						"Modes de transport",
						st.getForm().getSelectInput(
							PARAMETER_ROLLING_STOCK_FILTER,
							_site->getRollingStockFiltersList(),
							optional<size_t>(_rollingStockFilter->getRank())
					)	);
				}
				stream << st.cell("Transport public", st.getForm().getOuiNonRadioInput(PARAMETER_JOURNEY_PLANNING_ALGORITHM, _pt_journey_planning));
				stream << st.close();

				// No calculation without cities
				if (_startCity.empty() || _endCity.empty())
					return;

				if (_log)
					stream << "<h1>Trace</h1>";

				algorithm::PlanningOrder _planningOrder(DEPARTURE_FIRST);
				// Route planning
				RoadModule::ExtendedFetchPlaceResult startPlace(RoadModule::ExtendedFetchPlace(_site->getCitiesMatcher(), _startCity, _startPlace));
				RoadModule::ExtendedFetchPlaceResult endPlace(RoadModule::ExtendedFetchPlace(_site->getCitiesMatcher(), _endCity, _endPlace));

				ptime maxDepartureTime(_dateTime);
				if(_planningOrder == DEPARTURE_FIRST)
				{
					maxDepartureTime += days(1);
				}
				else
				{
					maxDepartureTime -= days(1);
				}
				ptime maxArrivalTime(maxDepartureTime);
				if(_planningOrder == DEPARTURE_FIRST)
				{
					maxArrivalTime += days(1);
					if(	startPlace.placeResult.value->getPoint().get() &&
						!startPlace.placeResult.value->getPoint()->isEmpty() &&
						endPlace.placeResult.value->getPoint().get() &&
						!endPlace.placeResult.value->getPoint()->isEmpty()
					){
						maxArrivalTime += minutes(2 * static_cast<int>(startPlace.placeResult.value->getPoint()->distance(endPlace.placeResult.value->getPoint().get()) / 1000));
					}
				}
				else
				{
					maxArrivalTime -= days(1);
					if(	startPlace.placeResult.value->getPoint().get() &&
						!startPlace.placeResult.value->getPoint()->isEmpty() &&
						endPlace.placeResult.value->getPoint().get() &&
						!endPlace.placeResult.value->getPoint()->isEmpty()
					){
						maxArrivalTime -= minutes(2 * static_cast<int>(startPlace.placeResult.value->getPoint()->distance(endPlace.placeResult.value->getPoint().get()) / 1000));
					}
				}

				stream << "<h1>Résultats</h1>";

				if(_pt_journey_planning)
				{
					PTTimeSlotRoutePlanner r(
						startPlace.placeResult.value.get(),
						endPlace.placeResult.value.get(),
						_planningOrder == DEPARTURE_FIRST ? _dateTime : maxArrivalTime,
						_planningOrder == DEPARTURE_FIRST ? maxDepartureTime : _dateTime,
						_planningOrder == DEPARTURE_FIRST ? _dateTime : maxDepartureTime,
						_planningOrder == DEPARTURE_FIRST ? maxArrivalTime : _dateTime,
						_resultsNumber,
						_site->getAccessParameters(_accessibility, _rollingStockFilter ? _rollingStockFilter->getAllowedPathClasses() : AccessParameters::AllowedPathClasses()),
						_planningOrder,
						false,
						_log ? &stream : NULL
						);
					const PTRoutePlannerResult jv(r.run());

					jv.displayHTMLTable(stream, optional<HTMLForm&>(), string(), false);
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
						startPlace.placeResult.value.get(),
						endPlace.placeResult.value.get(),
						_planningOrder == DEPARTURE_FIRST ? _dateTime : maxArrivalTime,
						_planningOrder == DEPARTURE_FIRST ? maxDepartureTime : _dateTime,
						_planningOrder == DEPARTURE_FIRST ? _dateTime : maxDepartureTime,
						_planningOrder == DEPARTURE_FIRST ? maxArrivalTime : _dateTime,
						1,
						ap,
						_planningOrder
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
			return user.getProfile()->isAuthorized<TransportWebsiteRight>(READ);
		}
		


		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(	dynamic_cast<const TransportWebsiteModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				TransportWebsiteTableSync::SearchResult sites(
					TransportWebsiteTableSync::Search(Env::GetOfficialEnv())
				);
				BOOST_FOREACH(shared_ptr<TransportWebsite> site, sites)
				{
					shared_ptr<TransportSiteAdmin> p(
						getNewPage<TransportSiteAdmin>()
					);
					p->_site = const_pointer_cast<const TransportWebsite>(site);
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
				BOOST_FOREACH(shared_ptr<Webpage> page, pages)
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

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("Périmètre base transport", TAB_PERIMETER, true));
			_tabs.push_back(Tab("Pages web", TAB_WEB_PAGES, true));
			_tabs.push_back(Tab("Calcul d'itinéraires", TAB_ROUTE_PLANNING, true));

			_tabBuilded = true;
		}
	}
}
