
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

#include "TransportSiteAdmin.h"

#include "AlgorithmLogger.hpp"
#include "TransportWebsiteModule.h"
#include "User.h"
#include "PTServiceConfigTableSync.hpp"
#include "SiteUpdateAction.h"
#include "TransportWebsiteRight.h"
#include "RoutePlannerFunction.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "ResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "HTMLModule.h"
#include "StaticFunctionRequest.h"
#include "Profile.h"
#include "JourneyPattern.hpp"
#include "SearchFormHTMLTable.h"
#include "HTMLForm.h"
#include "PTRoutePlannerResult.h"
#include "ActionResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "City.h"
#include "CityTableSync.h"
#include "PTPlacesAdmin.h"
#include "ObjectSiteLink.h"
#include "ObjectSiteLinkTableSync.h"
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
		const string TransportSiteAdmin::PARAMETER_SEARCH_RANK = "pr";
		const string TransportSiteAdmin::PARAMETER_JOURNEY_PLANNING_ALGORITHM = "ja";

		const string TransportSiteAdmin::TAB_PROPERTIES("pr");
		const string TransportSiteAdmin::TAB_PERIMETER("pe");
		const string TransportSiteAdmin::TAB_ROUTE_PLANNING("rp");



		TransportSiteAdmin::TransportSiteAdmin():
			AdminInterfaceElementTemplate<TransportSiteAdmin>(),
			_pt_journey_planning(true)
		{}



		void TransportSiteAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Config
			try
			{
				_config = PTServiceConfigTableSync::GetEditable(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
			}
			catch (ObjectNotFoundException<PTServiceConfig>&)
			{
				throw AdminParametersException("No such site");
			}

			// Journey planner
			_journeyPlanner.setConfiguration(_config);
			_journeyPlanner._setFromParametersMap(map);
			_journeyPlanner.setOutputFormat(RoutePlannerFunction::VALUE_ADMIN_HTML);

			// Use PT algorithm
			_pt_journey_planning = map.getDefault<bool>(PARAMETER_JOURNEY_PLANNING_ALGORITHM, true);
		}



		util::ParametersMap TransportSiteAdmin::getParametersMap() const
		{
			ParametersMap m(_journeyPlanner._getParametersMap());

			m.insert(PARAMETER_JOURNEY_PLANNING_ALGORITHM, _pt_journey_planning);
			if(_config.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _config->getKey());
			}
			return m;
		}



		void TransportSiteAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{

				// Requests
				AdminActionFunctionRequest<SiteUpdateAction, TransportSiteAdmin> updateRequest(
					_request,
					*this
				);
				updateRequest.getAction()->setSite(const_pointer_cast<PTServiceConfig>(_config));

				stream << "<h1>Propriétés</h1>";
				PropertiesHTMLTable pt(updateRequest.getHTMLForm());
				stream << pt.open();
				stream << pt.title("Identification");
				stream << pt.cell("Nom", pt.getForm().getTextInput(SiteUpdateAction::PARAMETER_NAME, _config->get<Name>()));
				stream << pt.title("Recherche d'itinéraires");
				stream << pt.cell("Max correspondances", pt.getForm().getSelectNumberInput(SiteUpdateAction::PARAMETER_MAX_CONNECTIONS, 0, 99, _config->get<MaxConnections>(), 1));
				stream << pt.cell("Réservation en ligne", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_ONLINE_BOOKING, _config->get<OnlineBookingActivated>()));
				stream << pt.cell("Affichage données passées", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_USE_OLD_DATA, _config->get<UseOldData>()));
				stream << pt.cell(
					"Nombre de jours chargés",
					pt.getForm().getSelectNumberInput(
						SiteUpdateAction::PARAMETER_USE_DATES_RANGE,
						1, 365,
						_config->get<UseDatesRange>().is_special() ?
							optional<size_t>() :
							optional<size_t>(_config->get<UseDatesRange>().days()),
						1,
						"illimité"
				)	);
				stream << pt.cell("Affichage détail approche routière", pt.getForm().getOuiNonRadioInput(SiteUpdateAction::PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL, _config->get<DisplayRoadApproachDetails>()));
				stream << pt.close();

				stream << "<h1>Périodes de recherche d'itinéraire</h1>";
				HTMLTable::ColsVector cv;
				cv.push_back("Nom");
				cv.push_back("Heure début");
				cv.push_back("Heure fin");
				HTMLTable ct(cv, ResultHTMLTable::CSS_CLASS);
				stream << ct.open();
				const HourPeriods::Type& periods(_config->get<HourPeriods>());
				BOOST_FOREACH(const HourPeriods::Type::value_type& period, periods)
				{
					stream << ct.row();
					stream << ct.col() << period.getCaption();
					stream << ct.col() << period.getBeginHour();
					stream << ct.col() << period.getEndHour();
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
						_config->getKey(),
						optional<RegistryKeyType>(),
						CityTableSync::TABLE.ID
				)	);

				AdminFunctionRequest<PTPlacesAdmin> openCityRequest(_request);

				AdminActionFunctionRequest<SiteCityAddAction,TransportSiteAdmin> cityAddRequest(_request, *this);
				cityAddRequest.getAction()->setConfig(_config);

				AdminActionFunctionRequest<RemoveObjectAction,TransportSiteAdmin> cityRemoveRequest(_request, *this);

				HTMLForm f(cityAddRequest.getHTMLForm("add_city"));
				HTMLTable::ColsVector v;
				v.push_back("Localité");
				v.push_back("Actions");
				v.push_back("Actions");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
				stream << f.open() << t.open();
				BOOST_FOREACH(const boost::shared_ptr<ObjectSiteLink>& link, cities)
				{
					cityRemoveRequest.getAction()->setObjectId(link->getKey());

					stream << t.row();
					if(Env::GetOfficialEnv().getRegistry<City>().contains(link->get<ObjectId>()))
					{
						boost::shared_ptr<const City> city(Env::GetOfficialEnv().get<City>(link->get<ObjectId>()));

						openCityRequest.getPage()->setCity(city);

						stream << t.col() << city->getName();
						stream << t.col() << HTMLModule::getLinkButton(openCityRequest.getURL(), "Ouvrir", string(), "/admin/img/" + PTPlacesAdmin::ICON);
					}
					else
					{
						stream << t.col() << "Inconsistent city " << link->get<ObjectId>();
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
			// TAB ROUTE PLANNER
			if (openTabContent(stream, TAB_ROUTE_PLANNING))
			{
				AdminFunctionRequest<TransportSiteAdmin> searchRequest(_request, *this);

				// Search form
				stream << "<h1>Recherche</h1>";

				SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
				stream << st.open();
				stream << st.cell("Commune départ", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT, _journeyPlanner.getDeparturePlace().cityResult.key.getSource()));
				stream << st.cell("Arrêt départ", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT, dynamic_cast<NamedPlace*>(_journeyPlanner.getDeparturePlace().placeResult.value.get()) ? _journeyPlanner.getDeparturePlace().placeResult.key.getSource() : string()));
				stream << st.cell("Commune arrivée", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT, _journeyPlanner.getArrivalPlace().cityResult.key.getSource()));
				stream << st.cell("Arrêt arrivée", st.getForm().getTextInput(RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT, dynamic_cast<NamedPlace*>(_journeyPlanner.getArrivalPlace().placeResult.value.get()) ? _journeyPlanner.getArrivalPlace().placeResult.key.getSource() : string()));
				stream << st.cell("Date/Heure", st.getForm().getCalendarInput(RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME, _journeyPlanner.getStartDepartureDate()));
				stream << st.cell(
					"Nombre réponses",
					st.getForm().getSelectNumberInput(
						RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER,
						1, 99,
						_journeyPlanner.getMaxSolutionsNumber(),
						1,
						"(illimité)"
				)	);
				stream << st.cell(
					"Accessibilité",
					st.getForm().getSelectInput(
						RoutePlannerFunction::PARAMETER_ACCESSIBILITY,
						TransportWebsiteModule::GetAccessibilityNames(),
						optional<UserClassCode>(_journeyPlanner.getAccessParameters().getUserClass())
				)	);
				stream << st.cell(
					"Trace",
					st.getForm().getTextInput(
						RoutePlannerFunction::PARAMETER_LOG_PATH,
						_journeyPlanner.getLogger().getDirectory().string()
				)	);
				if(!_config->getRollingStockFilters().empty())
				{
					stream << st.cell(
						"Modes de transport",
						st.getForm().getSelectInput(
							RoutePlannerFunction::PARAMETER_ROLLING_STOCK_FILTER_ID,
							_config->getRollingStockFiltersList(),
							_journeyPlanner.getTransportModeFilter() ? optional<size_t>(_journeyPlanner.getTransportModeFilter()->getRank()) : optional<size_t>()
					)	);
				}
				stream << st.cell("Transport public", st.getForm().getOuiNonRadioInput(PARAMETER_JOURNEY_PLANNING_ALGORITHM, _pt_journey_planning));
				stream << st.cell(
					"Durée max correspondance",
					st.getForm().getSelectNumberInput(
						RoutePlannerFunction::PARAMETER_MAX_TRANSFER_DURATION,
						1, 99,
						_journeyPlanner.getMaxTransferDuration() ?
							optional<size_t>(_journeyPlanner.getMaxTransferDuration()->total_seconds() / 60) :
							optional<size_t>(),
						1,
						"(illimité)"
				)	);
				stream << st.cell(
					"Vitesse d'approche",
					st.getForm().getTextInput(
						RoutePlannerFunction::PARAMETER_APPROACH_SPEED,
						lexical_cast<string>(_journeyPlanner.getAccessParameters().getApproachSpeed())
				)	);
				stream << st.close();

				stream << "<h1>Résultats</h1>";

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
						stream << HTMLModule::GetHTMLJavascriptOpen("/lib/openlayers/OpenLayers.js");
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
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const TransportWebsiteModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				PTServiceConfigTableSync::SearchResult sites(
					PTServiceConfigTableSync::Search(Env::GetOfficialEnv())
				);
				BOOST_FOREACH(const boost::shared_ptr<PTServiceConfig>& site, sites)
				{
					boost::shared_ptr<TransportSiteAdmin> p(
						getNewPage<TransportSiteAdmin>()
					);
					p->_config = site;
					links.push_back(p);
				}
			}
			return links;
		}



		AdminInterfaceElement::PageLinks TransportSiteAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			return links;
		}



		std::string TransportSiteAdmin::getTitle() const
		{
			return _config.get() ? _config->getName() : DEFAULT_TITLE;
		}




		bool TransportSiteAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _config->getKey() == static_cast<const TransportSiteAdmin&>(other)._config->getKey();
		}



		void TransportSiteAdmin::_buildTabs( const security::Profile& profile ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("Périmètre base transport", TAB_PERIMETER, true));
			_tabs.push_back(Tab("Calcul d'itinéraires", TAB_ROUTE_PLANNING, true));

			_tabBuilded = true;
		}
}	}
