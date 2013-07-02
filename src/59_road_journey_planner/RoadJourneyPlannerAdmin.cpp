
/** RoadJourneyPlannerAdmin class implementation.
	@file RoadJourneyPlannerAdmin.cpp
	@author Thomas Bonfort
	@date 2010

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

#include "RoadJourneyPlannerAdmin.h"

#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "Profile.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"

#include "ResultHTMLTable.h"
#include "RoadJourneyPlannerModule.hpp"
#include "RoadJourneyPlannerService.hpp"
#include "SearchFormHTMLTable.h"
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
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace security;
	using namespace graph;
	using namespace road_journey_planner;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, RoadJourneyPlannerAdmin>::FACTORY_KEY("RoadJourneyPlannerAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<RoadJourneyPlannerAdmin>::ICON("arrow_switch.png");
		template<> const string AdminInterfaceElementTemplate<RoadJourneyPlannerAdmin>::DEFAULT_TITLE("Itinéraires routiers");
	}

	namespace road_journey_planner
	{
		const std::string RoadJourneyPlannerAdmin::PARAMETER_START_PLACE("sp");
		const std::string RoadJourneyPlannerAdmin::PARAMETER_END_PLACE("ep");
		const std::string RoadJourneyPlannerAdmin::PARAMETER_ACCESSIBILITY("ac");


		RoadJourneyPlannerAdmin::RoadJourneyPlannerAdmin(
		):	AdminInterfaceElementTemplate<RoadJourneyPlannerAdmin>(),
			_accessibility(0)
		{ }



		void RoadJourneyPlannerAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_startPlace = map.getDefault<string>(PARAMETER_START_PLACE);
			_endPlace = map.getDefault<string>(PARAMETER_END_PLACE);
			_accessibility = map.getDefault<UserClassCode>(PARAMETER_ACCESSIBILITY, USER_PEDESTRIAN);
		}



		ParametersMap RoadJourneyPlannerAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAMETER_START_PLACE, _startPlace);
			m.insert(PARAMETER_END_PLACE, _endPlace);
			m.insert(PARAMETER_ACCESSIBILITY, static_cast<int>(_accessibility));
			return m;
		}



		void RoadJourneyPlannerAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const {

			// Search form
			stream << "<h1>Recherche</h1>";

			vector<pair<optional<UserClassCode>, string> > choices;
			choices.push_back(make_pair(optional<UserClassCode>(USER_PEDESTRIAN), "Piéton"));
			choices.push_back(make_pair(optional<UserClassCode>(USER_BIKE), "Vélo"));
			choices.push_back(make_pair(optional<UserClassCode>(USER_CAR), "Voiture"));

			AdminFunctionRequest<RoadJourneyPlannerAdmin> searchRequest(_request, *this);
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Lieu de d&eacute;part", st.getForm().getTextInput(PARAMETER_START_PLACE, _startPlace));
			stream << st.cell("Lieu d'arriv&eacute;e", st.getForm().getTextInput(PARAMETER_END_PLACE, _endPlace));
			stream << st.row();
			stream << st.cell("Mode de transport", st.getForm().getSelectInput(PARAMETER_ACCESSIBILITY, choices, optional<UserClassCode>(_accessibility ? _accessibility : USER_CAR)));

			stream << st.close();
			stream << st.getForm().setFocus(PARAMETER_START_PLACE);

			ParametersMap param;

			// No calculation without input
			if (_startPlace.empty() || _endPlace.empty())
				return;

			param.insert("dpt", _startPlace);
			param.insert("apt", _endPlace);
			param.insert("srid", 27572);
			param.insert("ac", static_cast<int>(_accessibility));

			RoadJourneyPlannerService r;
			r._setFromParametersMap(param);

			ParametersMap result(r.runWithoutOutput());

			string error = result.getDefault<string>("error_message");
			if(!error.empty())
			{
				if(error.compare("Departure or arrival place not found") == 0)
				{
					stream << "<p>Lieu de départ ou d'arrivée non trouvé.</p>" << endl;
				}
				else if(error.compare("No results") == 0)
				{
					stream << "<p>Aucun résultat trouvé.</p>" << endl;
				}
				else
				{
					stream << "<p>Erreur.</p>" << endl;
				}
			}
			else
			{
				stream << "<h2>Feuille de route</h2>" << endl;

				const boost::shared_ptr<ParametersMap> boardMap = *result.getSubMaps("boardMap").begin();

				stream << "<div>Trajet de " << boardMap->getValue("origin_city_text") << " " << boardMap->getValue("origin_place_text") << " à " << boardMap->getValue("destination_city_text") << " " << boardMap->getValue("destination_place_text") << ".</div>" << endl;
				stream << "Départ à " << boardMap->getValue("departure_time") << ", arrivée à " << boardMap->getValue("arrival_time") << " (" << boardMap->getValue("duration") << ", " << boardMap->getValue("distance") << "m).</div>" << endl;

				HTMLTable::ColsVector v;
				v.push_back("&Eacute;tape");
				v.push_back("Rue à prendre");
				v.push_back("Sur");
				v.push_back("Départ");
				v.push_back("Arriv&eacute;e");
				v.push_back("Distance totale");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);

				stream << t.open();
				stream << t.row() << endl;
				stream << t.col() << "0" << endl;
				stream << t.col() << boardMap->getValue("departure_name") << endl;
				stream << t.col() << "&nbsp;" << endl;
				stream << t.col() << boardMap->getValue("departure_time") << endl;
				stream << t.col() << "&nbsp;" << endl;
				stream << t.col() << "0m" << endl;

				size_t rank(0);

				if(boardMap->hasSubMaps("stepMap"))
				{
					BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& step, boardMap->getSubMaps("stepMap"))
					{
						stream << t.row() << endl;
						stream << t.col() << step->getValue("rank") << endl;
						stream << t.col() << step->getValue("road_name") << endl;
						stream << t.col() << step->getValue("distance") << "m" << endl;
						stream << t.col() << step->getValue("departure_time") << endl;
						stream << t.col() << step->getValue("arrival_time") << endl;
						stream << t.col() << step->getValue("total_distance") << "m" << endl;
						rank++;
					}
				}

				stream << t.row() << endl;
				stream << t.col() << ++rank << endl;
				stream << t.col() << boardMap->getValue("arrival_name") << endl;
				stream << t.col() << "&nbsp;";
				stream << t.col() << "&nbsp;" << endl;
				stream << t.col() << boardMap->getValue("arrival_time") << endl;
				stream << t.col() << boardMap->getValue("distance") << "m" << endl;
				stream << t.close() << endl;

				stream << "<h2>Carte</h2><div id=\"olmap\"></div>" << endl;
				stream << HTMLModule::GetHTMLJavascriptOpen();
				stream << "var tripWKT=\"" << result.getValue("wkt") << "\";";
				stream << HTMLModule::GetHTMLJavascriptClose();
				stream << HTMLModule::GetHTMLJavascriptOpen("http://svn.osgeo.org/metacrs/proj4js/trunk/lib/proj4js-combined.js");
				stream << HTMLModule::GetHTMLJavascriptOpen("http://svn.osgeo.org/metacrs/proj4js/trunk/lib/defs/EPSG900913.js");
				stream << HTMLModule::GetHTMLJavascriptOpen("http://trac.osgeo.org/proj4js/raw-attachment/ticket/32/EPSG27572.js");
				stream << HTMLModule::GetHTMLJavascriptOpen("/lib/openlayers/OpenLayers.js");
				stream << HTMLModule::GetHTMLJavascriptOpen("pedestrianroutemap.js");
			}
		}



		bool RoadJourneyPlannerAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}



		AdminInterfaceElement::PageLinks RoadJourneyPlannerAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(	dynamic_cast<const RoadJourneyPlannerModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		bool RoadJourneyPlannerAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			return true;
		}
	}
}
