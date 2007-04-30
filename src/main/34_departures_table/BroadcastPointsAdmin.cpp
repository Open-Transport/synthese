
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

#include <map>

#include "05_html/SearchFormHTMLTable.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/City.h"
#include "15_env/EnvModule.h"

#include "30_server/FunctionRequest.h"

#include "34_departures_table/BroadcastPointsAdmin.h"
#include "34_departures_table/BroadcastPointAdmin.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;

	namespace departurestable
	{
		const std::string BroadcastPointsAdmin::PARAMETER_CITY_NAME = "city";
		const std::string BroadcastPointsAdmin::PARAMETER_PLACE_NAME = "place";
		const std::string BroadcastPointsAdmin::PARAMETER_LINE_ID = "line";
		const std::string BroadcastPointsAdmin::PARAMETER_DISPLAY_NUMBER = "dpln";

		BroadcastPointsAdmin::BroadcastPointsAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) 
			, _displayNumber(WITH_OR_WITHOUT_ANY_BROADCASTPOINT)
			, _lineUId(UNKNOWN_VALUE)
		{}

		void BroadcastPointsAdmin::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it = map.find(PARAMETER_CITY_NAME);
			if (it != map.end())
				_cityName = it->second;

			it = map.find(PARAMETER_PLACE_NAME);
			if (it != map.end())
				_placeName = it->second;

			it = map.find(PARAMETER_DISPLAY_NUMBER);
			if (it != map.end())
				_displayNumber = (BroadcastPointsPresence) Conversion::ToInt(it->second);

			it = map.find(PARAMETER_LINE_ID);
			if (it != map.end())
				_lineUId = Conversion::ToLongLong(it->second);

			_requestParameters = ResultHTMLTable::getParameters(map, PARAMETER_CITY_NAME, 30);

			_searchResult = searchConnectionPlacesWithBroadcastPoints(
				_cityName
				, _placeName
				, _displayNumber
				, _lineUId
				, _requestParameters.maxSize
				, _requestParameters.first
				, _requestParameters.orderField == PARAMETER_CITY_NAME
				, _requestParameters.orderField == PARAMETER_PLACE_NAME
				, _requestParameters.orderField == PARAMETER_DISPLAY_NUMBER
				, _requestParameters.raisingOrder
				);

			_resultParameters = ResultHTMLTable::getParameters(_requestParameters, _searchResult);

		}

		string BroadcastPointsAdmin::getTitle() const
		{
			return "Points de diffusion";
		}

		void BroadcastPointsAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> goRequest(request);
			goRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());

			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointsAdmin>());

			vector<pair<int, string> > m;
			m.push_back(make_pair((int) WITH_OR_WITHOUT_ANY_BROADCASTPOINT, "(filtre désactivé)"));
			m.push_back(make_pair((int) AT_LEAST_ONE_BROADCASTPOINT, "Au moins un"));
			m.push_back(make_pair((int) NO_BROADCASTPOINT, "Aucun"));

			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune", st.getForm().getTextInput(PARAMETER_CITY_NAME, _cityName));
			stream << st.cell("Nom", st.getForm().getTextInput(PARAMETER_PLACE_NAME, _placeName));
			stream << st.cell("Terminaux d'affichage", st.getForm().getSelectInput(PARAMETER_DISPLAY_NUMBER, m, (int) _displayNumber));
			stream << st.cell("Ligne", st.getForm().getSelectInput(PARAMETER_LINE_ID, EnvModule::getCommercialLineLabels(true), _lineUId));
			stream << st.close();

			stream << "<h1>Résultats de la recherche</h1>";

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_CITY_NAME, "Commune"));
			h.push_back(make_pair(PARAMETER_PLACE_NAME, "Nom zone d'arrêt"));
			h.push_back(make_pair(PARAMETER_DISPLAY_NUMBER, "Afficheurs"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(h,searchRequest.getHTMLForm(), _requestParameters, _resultParameters);

			stream << t.open();


			for (vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> >::const_iterator it = _searchResult.begin(); it != _searchResult.end(); ++it)
			{
				goRequest.setObjectId((*it)->placeId);
				stream << t.row();
				stream << t.col() << (*it)->cityName;
				stream << t.col() << (*it)->placeName;
				stream << t.col() << (*it)->broadCastPointsNumber;
				
				HTMLForm gf(goRequest.getHTMLForm());
				stream << t.col() << gf.getLinkButton("Editer");
			}
			stream << t.close();
		}

		bool BroadcastPointsAdmin::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}

		std::string BroadcastPointsAdmin::getIcon() const
		{
			return "building.png";
		}
	}
}
