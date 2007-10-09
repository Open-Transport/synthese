
/** BroadcastPointsAdmin class implementation.
	@file BroadcastPointsAdmin.cpp

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

#include "34_departures_table/BroadcastPointsAdmin.h"
#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/ArrivalDepartureTableRight.h"

#include <map>

#include "05_html/SearchFormHTMLTable.h"

#include "15_env/ConnectionPlaceTableSync.h"
#include "15_env/City.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/EnvModule.h"

#include "30_server/FunctionRequest.h"
#include "30_server/Session.h"

#include "32_admin/HomeAdmin.h"

#include "12_security/User.h"
#include "12_security/Profile.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;
	using namespace departurestable;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, BroadcastPointsAdmin>::FACTORY_KEY("broadcastpoints");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BroadcastPointsAdmin>::ICON("building.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<BroadcastPointsAdmin>::DISPLAY_MODE(AdminInterfaceElement::EVER_DISPLAYED);
		template<> string AdminInterfaceElementTemplate<BroadcastPointsAdmin>::getSuperior()
		{
			return HomeAdmin::FACTORY_KEY;
		}
	}

	namespace departurestable
	{
		const std::string BroadcastPointsAdmin::PARAMETER_CITY_NAME = "city";
		const std::string BroadcastPointsAdmin::PARAMETER_PLACE_NAME = "place";
		const std::string BroadcastPointsAdmin::PARAMETER_LINE_ID = "line";
		const std::string BroadcastPointsAdmin::PARAMETER_DISPLAY_NUMBER = "dpln";

		BroadcastPointsAdmin::BroadcastPointsAdmin()
			: AdminInterfaceElementTemplate<BroadcastPointsAdmin>() 
			, _displayNumber(WITH_OR_WITHOUT_ANY_BROADCASTPOINT)
			, _lineUId(UNKNOWN_VALUE)
		{}

		void BroadcastPointsAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_cityName = map.getString(PARAMETER_CITY_NAME, false, FACTORY_KEY);
			_placeName = map.getString(PARAMETER_PLACE_NAME, false, FACTORY_KEY);
			
			int i(map.getInt(PARAMETER_DISPLAY_NUMBER, false, FACTORY_KEY));
			if (i != UNKNOWN_VALUE)
				_displayNumber = static_cast<BroadcastPointsPresence>(i);

			_lineUId = map.getUid(PARAMETER_LINE_ID, false, FACTORY_KEY);

			_requestParameters = ResultHTMLTable::getParameters(map.getMap(), PARAMETER_CITY_NAME, 30);
		}

		string BroadcastPointsAdmin::getTitle() const
		{
			return "Points de diffusion";
		}

		void BroadcastPointsAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			std::vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchResult(searchConnectionPlacesWithBroadcastPoints(
				request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
				, request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
				, READ
				, _cityName
				, _placeName
				, _displayNumber
				, _lineUId
				, _requestParameters.maxSize
				, _requestParameters.first
				, _requestParameters.orderField == PARAMETER_CITY_NAME
				, _requestParameters.orderField == PARAMETER_PLACE_NAME
				, _requestParameters.orderField == PARAMETER_DISPLAY_NUMBER
				, _requestParameters.raisingOrder
				));

			ResultHTMLTable::ResultParameters resultParameters(ResultHTMLTable::getParameters(_requestParameters, searchResult));

			FunctionRequest<AdminRequest> goRequest(request);
			goRequest.getFunction()->setPage<DisplaySearchAdmin>();

			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<BroadcastPointsAdmin>());

			vector<pair<int, string> > m;
			m.push_back(make_pair((int) WITH_OR_WITHOUT_ANY_BROADCASTPOINT, "(filtre désactivé)"));
			m.push_back(make_pair((int) AT_LEAST_ONE_BROADCASTPOINT, "Au moins un"));
			m.push_back(make_pair((int) NO_BROADCASTPOINT, "Aucun"));

			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune", st.getForm().getTextInput(PARAMETER_CITY_NAME, _cityName));
			stream << st.cell("Nom", st.getForm().getTextInput(PARAMETER_PLACE_NAME, _placeName));
			stream << st.cell("Terminaux d'affichage", st.getForm().getSelectInput(PARAMETER_DISPLAY_NUMBER, m, static_cast<int>(_displayNumber)));
			stream << st.cell("Ligne", st.getForm().getSelectInput(
				PARAMETER_LINE_ID
				, EnvModule::getCommercialLineLabels(
					request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
					, request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
					, READ
					, true)
				, _lineUId)
				);
			stream << st.close();

			stream << "<h1>Résultats de la recherche</h1>";

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_CITY_NAME, "Commune"));
			h.push_back(make_pair(PARAMETER_PLACE_NAME, "Nom zone d'arrêt"));
			h.push_back(make_pair(PARAMETER_DISPLAY_NUMBER, "Afficheurs"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(h,st.getForm(), _requestParameters, resultParameters);

			stream << t.open();


			for (vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> >::const_iterator it = searchResult.begin(); it != searchResult.end(); ++it)
			{
				stream << t.row();
				try
				{
					shared_ptr<PublicTransportStopZoneConnectionPlace> place((*it)->place);
					stream << t.col() << (*it)->cityName;
					stream << t.col() << place->getName();
					stream << t.col() << (*it)->broadCastPointsNumber;
					HTMLForm gf(goRequest.getHTMLForm());
					gf.addHiddenField(DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION_ID, Conversion::ToString(place->getKey()));
					stream << t.col() << gf.getLinkButton("Editer", string(), "building_edit.png");
				}
				catch (...)
				{
					stream << t.col(3) << HTMLModule::getHTMLImage("exclamation.png", "Erreur de données") << " Erreur de données : arrêt inexistant";
				}
			}
			stream << t.close();
		}

		bool BroadcastPointsAdmin::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return request->isAuthorized<ArrivalDepartureTableRight>(READ);
		}
	}
}
