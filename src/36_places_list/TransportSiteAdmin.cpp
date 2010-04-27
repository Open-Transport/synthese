
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
#include "RoutePlannerInterfacePage.h"
#include "Profile.h"
#include "StaticFunctionRequest.h"
#include "Profile.h"
#include "Line.h"
#include "PlacesListModule.h"
#include "Site.h"
#include "SiteTableSync.h"
#include "PTTimeSlotRoutePlanner.h"
#include "SearchFormHTMLTable.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "HTMLForm.h"
#include "PTRoutePlannerResult.h"
#include "WebPageAdmin.h"
#include "ActionResultHTMLTable.h"
#include "WebPageAddAction.h"
#include "WebPageRemoveAction.h"
#include "AdminFunctionRequest.hpp"
#include "WebPageDisplayFunction.h"
#include "StaticFunctionRequest.h"
#include "WebPageRemoveAction.h"
#include "City.h"
#include "CityTableSync.h"
#include "PTPlacesAdmin.h"
#include "ObjectSiteLink.h"
#include "ObjectSiteLinkTableSync.h"

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
	using namespace transportwebsite;
	using namespace html;
	using namespace routeplanner;
	using namespace security;
	using namespace ptrouteplanner;
	using namespace algorithm;
	using namespace html;
	using namespace pt;
	using namespace graph;
	using namespace geography;
	using namespace pt;

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

		const string TransportSiteAdmin::TAB_PROPERTIES("pr");
		const string TransportSiteAdmin::TAB_PERIMETER("pe");
		const string TransportSiteAdmin::TAB_ROUTE_PLANNING("rp");
		const string TransportSiteAdmin::TAB_WEB_PAGES("wp");
		
		TransportSiteAdmin::TransportSiteAdmin()
			: AdminInterfaceElementTemplate<TransportSiteAdmin>(),
			_dateTime(not_a_date_time),
			_accessibility(USER_PEDESTRIAN),
			_log(false),
			_rollingStockFilter(NULL)

		{ }
		
		void TransportSiteAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_site = SiteTableSync::Get(
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
					Site::RollingStockFilters::const_iterator it(_site->getRollingStockFilters().find(map.get<size_t>(PARAMETER_ROLLING_STOCK_FILTER)));
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
			VariablesMap& variables,
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
				const Site::Periods& periods(_site->getPeriods());
				for (Site::Periods::const_iterator it(periods.begin()); it != periods.end(); ++it)
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

				ObjectSiteLinkTableSync::SearchResult cities(ObjectSiteLinkTableSync::Search(_getEnv(), _site->getKey(), optional<RegistryKeyType>(), CityTableSync::TABLE.ID));

				AdminFunctionRequest<PTPlacesAdmin> openCityRequest(_request);

				HTMLTable::ColsVector v;
				v.push_back("Localité");
				v.push_back("Action");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(shared_ptr<ObjectSiteLink> link, cities)
				{
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
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB WEB PAGES
			if (openTabContent(stream, TAB_WEB_PAGES))
			{
				
				stream << "<h1>Recherche</h1>";
				AdminFunctionRequest<TransportSiteAdmin> searchRequest(_request);
				SearchFormHTMLTable sft(searchRequest.getHTMLForm("pagesearch"));
				stream << sft.open();
				stream << sft.cell("Titre", sft.getForm().getTextInput(PARAMETER_SEARCH_PAGE, _searchPage));
				stream << sft.close();

				stream << "<h1>Résultat</h1>";
				AdminActionFunctionRequest<WebPageAddAction, TransportSiteAdmin> addRequest(_request);
				addRequest.getAction()->setSite(const_pointer_cast<Site>(_site));

				AdminActionFunctionRequest<WebPageAddAction, TransportSiteAdmin> parentAddRequest(_request);
				addRequest.getAction()->setSite(const_pointer_cast<Site>(_site));

				AdminActionFunctionRequest<WebPageRemoveAction, TransportSiteAdmin> deleteRequest(_request);

				AdminFunctionRequest<WebPageAdmin> openRequest(_request);

				StaticFunctionRequest<WebPageDisplayFunction> viewRequest(_request, false);
				if(	!_site->getClientURL().empty()
				){
					viewRequest.setClientURL(_site->getClientURL());
				}
				
				WebPageTableSync::SearchResult result(
					WebPageTableSync::Search(
						Env::GetOfficialEnv(),
						_site->getKey(),
						RegistryKeyType(0),
						_pageSearchParameter.first,
						_pageSearchParameter.maxSize,
						_pageSearchParameter.orderField == PARAMETER_SEARCH_RANK,
						_pageSearchParameter.orderField == PARAMETER_SEARCH_PAGE,
						_pageSearchParameter.raisingOrder
				)	);

				ActionResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(PARAMETER_SEARCH_RANK, "#"));
				h.push_back(make_pair(PARAMETER_SEARCH_PAGE, "Titre"));
				h.push_back(make_pair(string(), "Actions"));
				h.push_back(make_pair(string(), "Actions"));
				h.push_back(make_pair(string(), "Actions"));
				h.push_back(make_pair(string(), "Actions"));
				ActionResultHTMLTable t(h, sft.getForm(), _pageSearchParameter, result, addRequest.getHTMLForm(), WebPageAddAction::PARAMETER_TEMPLATE_ID);
				stream << t.open();

				_displaySubPages(stream, result, openRequest, viewRequest, parentAddRequest, deleteRequest, t);

				stream << t.row();
				stream << t.col();
				stream << t.col() << t.getActionForm().getTextInput(WebPageAddAction::PARAMETER_TITLE, string(), "(Entrez le titre ici)");
				stream << t.col(4) << t.getActionForm().getSubmitButton("Créer");
				stream << t.close();
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
					st.getForm().getSelectInput(PARAMETER_ACCESSIBILITY, PlacesListModule::GetAccessibilityNames(), optional<UserClassCode>(_accessibility)));
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
				stream << st.close();

				// No calculation without cities
				if (_startCity.empty() || _endCity.empty())
					return;

				if (_log)
					stream << "<h1>Trace</h1>";

				ptime endDate(_dateTime);
				endDate += days(1);

				// Route planning
				const Place* startPlace(_site->fetchPlace(_startCity, _startPlace));
				const Place* endPlace(_site->fetchPlace(_endCity, _endPlace));
				PTTimeSlotRoutePlanner r(
					startPlace,
					endPlace,
					_dateTime,
					endDate,
					_dateTime,
					endDate,
					_resultsNumber,
					_site->getAccessParameters(_accessibility, _rollingStockFilter ? _rollingStockFilter->getAllowedPathClasses() : AccessParameters::AllowedPathClasses()),
					DEPARTURE_FIRST,
					_log ? &stream : NULL
				);
				const PTRoutePlannerResult jv(r.run());

				stream << "<h1>Résultats</h1>";

				jv.displayHTMLTable(stream, optional<HTMLForm&>(), string());

			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		void TransportSiteAdmin::_displaySubPages(
			std::ostream& stream,
			const WebPageTableSync::SearchResult& pages,
			AdminFunctionRequest<WebPageAdmin>& openRequest,
			StaticFunctionRequest<WebPageDisplayFunction>& viewRequest,
			AdminActionFunctionRequest<WebPageAddAction, TransportSiteAdmin>& createRequest,
			AdminActionFunctionRequest<WebPageRemoveAction, TransportSiteAdmin>& deleteRequest,
			ActionResultHTMLTable& t,
			size_t depth
		) const {

			BOOST_FOREACH(shared_ptr<WebPage> page, pages)
			{
				openRequest.getPage()->setPage(const_pointer_cast<const WebPage>(page));
				viewRequest.getFunction()->setPage(const_pointer_cast<const WebPage>(page));
				deleteRequest.getAction()->setPage(page);

				stream << t.row(lexical_cast<string>(page->getKey()));
				stream << t.col();
				for(size_t i(0); i<depth; ++i)
					stream << "&nbsp;&nbsp;&nbsp;";
				stream << page->getRank();
				stream << t.col() << page->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), WebPageAdmin::ICON);
				stream << t.col() << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "page_go.png");
				
				WebPageTableSync::SearchResult result(
					WebPageTableSync::Search(
						Env::GetOfficialEnv(),
						_site->getKey(),
						page->getKey(),
						_pageSearchParameter.first,
						_pageSearchParameter.maxSize,
						_pageSearchParameter.orderField == PARAMETER_SEARCH_RANK,
						_pageSearchParameter.orderField == PARAMETER_SEARCH_PAGE,
						_pageSearchParameter.raisingOrder
				)	);

				stream << t.col();
				if(result.empty())
				{
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la page "+ page->getName() +" ?", "page_delete.png");
				}

				stream << t.col();
				if(result.empty())
				{
					createRequest.getAction()->setParent(page);
					stream << HTMLModule::getLinkButton(createRequest.getURL(), "Créer sous-page", string(), "page_add.png");
				}

				_displaySubPages(stream, result, openRequest, viewRequest, createRequest, deleteRequest, t, depth+1);
			}
		}



		bool TransportSiteAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportWebsiteRight>(READ);
		}
		


		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPagesOfModule(
			const string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(moduleKey == PlacesListModule::FACTORY_KEY && request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				SiteTableSync::SearchResult sites(
					SiteTableSync::Search(*_env)
				);
				BOOST_FOREACH(shared_ptr<Site> site, sites)
				{
					shared_ptr<TransportSiteAdmin> p(
						getNewOtherPage<TransportSiteAdmin>(false)
					);
					p->_site = const_pointer_cast<const Site>(site);
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
				currentPage.getCurrentTreeBranch().find(*this)
			){
				WebPageTableSync::SearchResult pages(WebPageTableSync::Search(Env::GetOfficialEnv(), _site->getKey(), RegistryKeyType(0)));
				BOOST_FOREACH(shared_ptr<WebPage> page, pages)
				{
					shared_ptr<WebPageAdmin> p(
						getNewOtherPage<WebPageAdmin>(false)
					);
					p->setPage(const_pointer_cast<const WebPage>(page));
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



		boost::shared_ptr<const Site> TransportSiteAdmin::getSite() const
		{
			return _site;
		}



		void TransportSiteAdmin::setSite( boost::shared_ptr<const Site> value )
		{
			_site = value;
		}
	}
}
