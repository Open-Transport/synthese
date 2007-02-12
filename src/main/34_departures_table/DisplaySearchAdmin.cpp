
/** DisplaySearchAdmin class implementation.
	@file DisplaySearchAdmin.cpp

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

#include "01_util/Html.h"
#include "01_util/Conversion.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/EnvModule.h"

#include "32_admin/AdminRequest.h"

#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/CreateDisplayScreenAction.h"
#include "34_departures_table/DisplayAdmin.h"
#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreenContentRequest.h"
#include "34_departures_table/DeparturesTableModule.h"


using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;

	namespace departurestable
	{
		const string DisplaySearchAdmin::PARAMETER_SEARCH_UID = "dsasuid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LINE_ID = "dsaslid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION = "dsasloc";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_TYPE_ID = "dsasti";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STATE = "dsass";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_MESSAGE = "dsasm";

		DisplaySearchAdmin::DisplaySearchAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED)
			, _searchLocalizationUId(UNKNOWN_VALUE)
			, _searchLineId(UNKNOWN_VALUE)
			, _searchTypeId(UNKNOWN_VALUE)
			, _searchState(UNKNOWN_VALUE)
			, _searchMessage(UNKNOWN_VALUE)
		{}

		void DisplaySearchAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map.find(PARAMETER_SEARCH_UID);
			if (it != map.end())
				_searchUId = it->second;

			it = map.find(PARAMETER_SEARCH_LOCALIZATION);
			if (it != map.end())
				_searchLocalizationUId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_LINE_ID);
			if (it != map.end())
				_searchLineId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_TYPE_ID);
			if (it != map.end())
				_searchTypeId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_STATE);
			if (it != map.end())
				_searchState = Conversion::ToInt(it->second);

			it = map.find(PARAMETER_SEARCH_MESSAGE);
			if (it != map.end())
				_searchMessage = Conversion::ToInt(it->second);

			_result = DisplayScreenTableSync::search(_searchUId, _searchLocalizationUId, _searchLineId, _searchTypeId, _searchState, _searchMessage);
		}

		string DisplaySearchAdmin::getTitle() const
		{
			return "Afficheurs";
		}

		void DisplaySearchAdmin::display(ostream& stream, const Request* request) const
		{
			AdminRequest* createDisplayRequest = Factory<Request>::create<AdminRequest>();
			createDisplayRequest->copy(request);
			createDisplayRequest->setAction(Factory<Action>::create<CreateDisplayScreenAction>());
			createDisplayRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			createDisplayRequest->setActionFailedPage(Factory<AdminInterfaceElement>::create<DisplaySearchAdmin>());

			AdminRequest* searchRequest =  Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<DisplaySearchAdmin>());

			AdminRequest* updateRequest =  Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());

			DisplayScreenContentRequest* viewRequest =  Factory<Request>::create<DisplayScreenContentRequest>();
			viewRequest->copy(request);

			AdminRequest* maintRequest =  Factory<Request>::create<AdminRequest>();
			maintRequest->copy(request);
			maintRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayMaintenanceAdmin>());


			map<int, string> states;
			states.insert(make_pair(UNKNOWN_VALUE, "(tous)"));
			states.insert(make_pair(UNKNOWN_VALUE, "OK"));
			states.insert(make_pair(UNKNOWN_VALUE, "Warning"));
			states.insert(make_pair(UNKNOWN_VALUE, "Warning+Error"));
			states.insert(make_pair(UNKNOWN_VALUE, "Error"));

			map<int, string> messages;
			messages.insert(make_pair(UNKNOWN_VALUE, "(tous)"));
			messages.insert(make_pair(UNKNOWN_VALUE, "Un message"));
			messages.insert(make_pair(UNKNOWN_VALUE, "Conflit"));
			messages.insert(make_pair(UNKNOWN_VALUE, "Messages"));

			stream
				<< "<h1>Recherche</h1>"
				<< searchRequest->getHTMLFormHeader("search")
				<< "<table>"
				<< "<tr><td>UID</td><td>" << Html::getTextInput(PARAMETER_SEARCH_UID, _searchUId) << "</td>"
				<< "<td>Emplacement</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_LOCALIZATION, DeparturesTableModule::getPlacesWithBroadcastPointsLabels(true), _searchLocalizationUId) << "</td>"
				<< "<td>Ligne</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_LINE_ID, EnvModule::getCommercialLineLabels(true), _searchLineId) << "</td></tr>"
				<< "<tr><td>Type</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_TYPE_ID, DeparturesTableModule::getDisplayTypeLabels(true), _searchTypeId) << "</td>"
				<< "<td>Etat</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_TYPE_ID, states, _searchState) << "</td>"
				<< "<td>Message</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_MESSAGE, messages, _searchMessage) << "</td></tr>"
				<< "<tr><td colspan=\"6\">" << Html::getSubmitButton("Rechercher") << "</td></tr>"
				<< "</table></form>"
				
				<< "<h1>Résultats de la recherche</h1>"
				<< "<table>"
				<< "<tr><th>Sel</th><th>UID</th><th>Emplacement</th><th>Type</th><th>Etat</th><th>Msg</th><th colspan=\"3\">Actions</th></tr>";

			for (vector<DisplayScreen*>::const_iterator it = _result.begin(); it != _result.end(); ++it)
			{
				DisplayScreen* screen = *it;
				updateRequest->setObjectId(screen->getKey());
				viewRequest->setDisplayScreen(screen);
				maintRequest->setObjectId(screen->getKey());
				stream
					<< "<tr><td><INPUT type=\"checkbox\" name=\"Checkbox3\"></td>"
					<< "<td>" << screen->getKey() << "</td>"
					<< "<td>" << (screen->getLocalization() ? screen->getLocalization()->getName() : "(indéterminé)") << "</td>"
					<< "<td>" << (screen->getType() ? screen->getType()->getName() : "(indéterminé)") << "</td>"
					<< "<td></td>" // Bullets showing the states of the display
					<< "<td></td>" // Bullet showing the message status
					<< "<td>" << updateRequest->getHTMLFormHeader("update" + Conversion::ToString(screen->getKey()))
					<< Html::getSubmitButton("Modifier") << "</form></td>"
					<< "<td>" << viewRequest->getHTMLFormHeader("view" + Conversion::ToString(screen->getKey()))
					<< Html::getSubmitButton("Simuler") << "</form></td>"
					<< "<td>" << maintRequest->getHTMLFormHeader("maint" + Conversion::ToString(screen->getKey()))
					<< Html::getSubmitButton("Supervision") << "</form></td>"
					<< "</tr>";
			}

			stream
				<< "<tr>"
				<< "<td colspan=\"6\">(sélectionner un afficheur existant pour copier ses&nbsp;propriétés dans le nouvel afficheur)</td>"
				<< "<td colspan=\"3\">" << createDisplayRequest->getHTMLFormHeader("add")
				<< Html::getSubmitButton("Créer un nouvel afficheur")
				<< "</form></td>";

			delete createDisplayRequest;
			delete searchRequest;
			delete updateRequest;
			delete viewRequest;
			delete maintRequest;
		}
	}
}
