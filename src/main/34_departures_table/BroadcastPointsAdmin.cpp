
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

#include "01_util/Html.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/City.h"
#include "15_env/EnvModule.h"

#include "34_departures_table/BroadcastPointsAdmin.h"
#include "34_departures_table/BroadcastPointAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace env;

	namespace departurestable
	{
		const std::string BroadcastPointsAdmin::PARAMETER_CITY_NAME = "city";
		const std::string BroadcastPointsAdmin::PARAMETER_PLACE_NAME = "place";
		const std::string BroadcastPointsAdmin::PARAMETER_LINE_ID = "line";
		const std::string BroadcastPointsAdmin::PARAMETER_DISPLAY_NUMBER = "dpln";
		const std::string BroadcastPointsAdmin::PARAMETER_NUMBER = "n";
		const std::string BroadcastPointsAdmin::PARAMETER_FIRST = "f";

		BroadcastPointsAdmin::BroadcastPointsAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) 
			, _displayNumber(WITH_OR_WITHOU_ANY_BROADCASTPOINT), _number(50), _first(0)
		{}

		void BroadcastPointsAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map.find(PARAMETER_CITY_NAME);
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

			it = map.find(PARAMETER_NUMBER);
			if (it != map.end())
				_number = Conversion::ToInt(it->second);

			it = map.find(PARAMETER_FIRST);
			if (it != map.end())
				_first = Conversion::ToInt(it->second);

			_searchResult = searchConnectionPlacesWithBroadcastPoints(_cityName, _placeName, _displayNumber, _lineUId, _number, _first);

		}

		string BroadcastPointsAdmin::getTitle() const
		{
			return "Points de diffusion";
		}

		void BroadcastPointsAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* goRequest = Factory<Request>::create<AdminRequest>();
			goRequest->copy(request);
			goRequest->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());

			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointsAdmin>());

			map<int, string> m;
			m.insert(make_pair((int) WITH_OR_WITHOU_ANY_BROADCASTPOINT, "(filtre désactivé)"));
			m.insert(make_pair((int) AT_LEAST_ONE_BROADCASTPOINT, "Au moins un"));
			m.insert(make_pair((int) NO_BROADCASTPOINT, "Aucun"));

			stream
				<< "<h1>Recherche</h1>"
				<< searchRequest->getHTMLFormHeader("search")
				<< "<table>"
				<< "<tr><td>Commune</td><td>" << Html::getTextInput(PARAMETER_CITY_NAME, _cityName) << "</td>"
				<< "<td>Nom</td><td>" << Html::getTextInput(PARAMETER_PLACE_NAME, _placeName) << "</td>"
				<< "<td>Terminaux d'affichage</td><td>" << Html::getSelectInput(PARAMETER_DISPLAY_NUMBER, m, (int) _displayNumber) << "</td></tr>"
				<< "<tr><td>Ligne</td><td>" 
				<< Html::getSelectInput(PARAMETER_LINE_ID, EnvModule::getCommercialLineLabels(true), _lineUId) << "</td>"	// Lines list
				<< "<td colspan=\"4\">" << Html::getSubmitButton("Rechercher") << "</td></tr>"
				<< "</table></form>"

				<< "<h1>Résultats de la recherche</h1>"
				<< "<table>"
				<< "<tr><th>Commune</th><th>Nom zone d'arrêt</th><th>Afficheurs</th><th>Actions</th></tr>";


			for (vector<ConnectionPlaceWithBroadcastPoint>::const_iterator it = _searchResult.begin(); it != _searchResult.end(); ++it)
			{
				goRequest->setObjectId(it->place->getKey());
				stream
					<< "<tr><td>" << it->place->getCity()->getName() << "</td>"
					<< "<td>" << it->place->getName() << "</td>"
					<< "<td>" << it->broadCastPointsNumber << "</td>"
					<< "<td>"
					<< goRequest->getHTMLFormHeader(Conversion::ToString(it->place->getKey()))
					<< Html::getSubmitButton("Editer") 
					<< "</form></td>"
					<< "</tr>";
			}
			stream
				<< "</table>"
				<< "<table style=\"border:none\"><tr><td>";
			if (_first > 0)
			{
				searchRequest->setParameter(PARAMETER_FIRST, Conversion::ToString((_first > _number) ? _first - _number : 0));
				stream << searchRequest->getHTMLLink("Emplacements précédents");
			}
			stream << "</td><td style=\"text-align:right\">";
			if (_searchResult.size() >= _number)
			{
				searchRequest->setParameter(PARAMETER_FIRST, Conversion::ToString(_first + _number));
				stream << searchRequest->getHTMLLink("Emplacements suivants");
			}
			stream << "</td></tr></table>";

			delete goRequest;
			delete searchRequest;
		}

		BroadcastPointsAdmin::~BroadcastPointsAdmin()
		{
		}
	}
}
