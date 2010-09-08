
/** RoadJourneyPlannerAdmin class implementation.
	@file RoadJourneyPlannerAdmin.cpp
	@author Thomas Bonfort
	@date 2010

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

#include "RoadJourneyPlannerAdmin.h"
#include "GeographyModule.h"
#include "NamedPlace.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "Profile.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"

#include "RoadJourneyPlanner.h"
#include "RoadJourneyPlannerResult.h"
#include "RoadJourneyPlannerModule.hpp"
#include "SearchFormHTMLTable.h"
#include "PTConstants.h"
#include "Journey.h"
#include "ServicePointer.h"
#include "Road.h"
#include "RoadChunk.h"
#include "AccessParameters.h"
#include "RoadPlace.h"
#include "User.h"
#include "UserTableSync.h"

#include <boost/date_time/posix_time/posix_time.hpp>

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
	using namespace html;
	using namespace algorithm;	
	using namespace security;
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace road_journey_planner;
	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, RoadJourneyPlannerAdmin>::FACTORY_KEY("RoadJourneyPlannerAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<RoadJourneyPlannerAdmin>::ICON("arrow_switch.png");
		template<> const string AdminInterfaceElementTemplate<RoadJourneyPlannerAdmin>::DEFAULT_TITLE("Itineraires pi&eacute;tons");
	}

	namespace road_journey_planner
	{
		const std::string RoadJourneyPlannerAdmin::PARAMETER_START_CITY("sc");
		const std::string RoadJourneyPlannerAdmin::PARAMETER_START_PLACE("sp");
		const std::string RoadJourneyPlannerAdmin::PARAMETER_END_CITY("ec");
		const std::string RoadJourneyPlannerAdmin::PARAMETER_END_PLACE("ep");
		const std::string RoadJourneyPlannerAdmin::PARAMETER_PLANNING_ORDER("po");



		RoadJourneyPlannerAdmin::RoadJourneyPlannerAdmin(
		):	AdminInterfaceElementTemplate<RoadJourneyPlannerAdmin>(),
			_planningOrder(DEPARTURE_FIRST)
		{ }
		


		void RoadJourneyPlannerAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_planningOrder = static_cast<PlanningOrder>(map.getDefault<int>(PARAMETER_PLANNING_ORDER));
			_startCity = map.getDefault<string>(PARAMETER_START_CITY);
			_startPlace = map.getDefault<string>(PARAMETER_START_PLACE);
			_endCity = map.getDefault<string>(PARAMETER_END_CITY);
			_endPlace = map.getDefault<string>(PARAMETER_END_PLACE);

		}
		
		
		
		ParametersMap RoadJourneyPlannerAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAMETER_START_CITY, _startCity);
			m.insert(PARAMETER_START_PLACE, _startPlace);
			m.insert(PARAMETER_END_CITY, _endCity);
			m.insert(PARAMETER_END_PLACE, _endPlace);
			m.insert(PARAMETER_PLANNING_ORDER, static_cast<int>(_planningOrder));
			return m;
		}

		
		
		void RoadJourneyPlannerAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& _request
		) const {

			// Search form
			stream << "<h1>Recherche</h1>";

			const Place* startPlace(NULL);
			const Place* endPlace(NULL);

			if (!_startCity.empty() && !_endCity.empty())
			{
				startPlace = GeographyModule::FetchPlace(_startCity, _startPlace);
				endPlace = GeographyModule::FetchPlace(_endCity, _endPlace);
			}

			AdminFunctionRequest<RoadJourneyPlannerAdmin> searchRequest(_request);
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune d&eacute;part", st.getForm().getTextInput(
						PARAMETER_START_CITY, 
						startPlace ? 
						(dynamic_cast<const City*>(startPlace) ? dynamic_cast<const City*>(startPlace)->getName() : dynamic_cast<const NamedPlace*>(startPlace)->getCity()->getName()) :
						_startCity
				)	);
			stream << st.cell("Lieu d&eacute;part", st.getForm().getTextInput(
						PARAMETER_START_PLACE,
						startPlace ? 
						(dynamic_cast<const City*>(startPlace) ? string() : dynamic_cast<const NamedPlace*>(startPlace)->getName()) :
						_startPlace
				)	);
			stream << st.row();
			stream << st.cell("Commune arriv&eacute;e", st.getForm().getTextInput(
						PARAMETER_END_CITY,
						endPlace ? 
						(dynamic_cast<const City*>(endPlace) ? dynamic_cast<const City*>(endPlace)->getName() : dynamic_cast<const NamedPlace*>(endPlace)->getCity()->getName()) :
						_endCity
				)	);	
			stream << st.cell("Lieu arriv&eacute;e", st.getForm().getTextInput(
						PARAMETER_END_PLACE,
						endPlace ? 
						(dynamic_cast<const City*>(endPlace) ? string() : dynamic_cast<const NamedPlace*>(endPlace)->getName()) :
						_endPlace
				)	);
			stream << st.close();
			stream << st.getForm().setFocus(PARAMETER_START_CITY);

			// No calculation without cities
			if (_startCity.empty() || _endCity.empty())
				return;

			ptime now(second_clock::local_time());


			ptime endDate(now);
			if(_planningOrder == DEPARTURE_FIRST)
			{
				endDate += days(1);
			}
			else
			{
				endDate -= days(1);
			}

			// Route planning
			AccessParameters ap(
				USER_PEDESTRIAN,
				false, false, 30000, posix_time::hours(5), 1.111,
				1000
			);
			RoadJourneyPlanner r(
				startPlace,
				endPlace,
				_planningOrder == DEPARTURE_FIRST ? now : endDate,
				_planningOrder == DEPARTURE_FIRST ? endDate : now,
				_planningOrder == DEPARTURE_FIRST ? now : endDate,
				_planningOrder == DEPARTURE_FIRST ? endDate : now,
				1,
				ap,
				_planningOrder
			);
			RoadJourneyPlannerResult jv(r.run());


			stream << "<h1>R&eacute;sultats</h1>";

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

		bool RoadJourneyPlannerAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}
		
		AdminInterfaceElement::PageLinks RoadJourneyPlannerAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(moduleKey == RoadJourneyPlannerModule::FACTORY_KEY && request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewPage());
			}
			return links;
		}
		
		
		
		bool RoadJourneyPlannerAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			return true;
		}
	}
}
