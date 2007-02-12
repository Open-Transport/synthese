
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

#include "15_env/ConnectionPlace.h"

#include "32_admin/AdminRequest.h"

#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/CreateDisplayScreenAction.h"
#include "34_departures_table/DisplayAdmin.h"


using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace departurestable
	{
		const string DisplaySearchAdmin::PARAMETER_SEARCH_UID = "dsasuid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION = "dsasloc";

		DisplaySearchAdmin::DisplaySearchAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		void DisplaySearchAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map.find(PARAMETER_SEARCH_UID);
			if (it != map.end())
				_searchUId = it->second;

			it = map.find(PARAMETER_SEARCH_LOCALIZATION);
			if (it != map.end())
				_searchLocalizationUId = it->second;
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


			map<string, string> localizations;
			localizations.insert(make_pair("", "(tous)"));
			std::vector<ConnectionPlaceWithBroadcastPoint> bpv = searchConnectionPlacesWithBroadcastPoints("", "", AT_LEAST_ONE_BROADCASTPOINT);
			for (vector<ConnectionPlaceWithBroadcastPoint>::const_iterator it = bpv.begin(); it != bpv.end(); ++it)
				localizations.insert(make_pair(Conversion::ToString(it->place->getKey()), it->place->getFullName()));
			

			stream
				<< "<table>"
				<< "<tr><td>UID</td><td>" << Html::getTextInput(PARAMETER_SEARCH_UID, _searchUId) << "</td>"
				<< "<td>Emplacement</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_LOCALIZATION, localizations, _searchLocalizationUId) << "</td>"
				<< "<td>Ligne</td><td><select name=\"Select4\">"
				<< "<option value=\"\">(toutes)</option>";

			// List of lines

			stream
				<< "</SELECT></td></tr>"
				<< "<tr><td>Type</td><td><SELECT name=\"Select2\">"
				<< "<OPTION value=\"\">(tous)</OPTION>";

			// List of display types

			stream
				<< "</SELECT></td><td>Etat</td><td><SELECT name=\"Select3\">"
				<< "<OPTION value=\"\">(tous)</OPTION>"
				<< "<OPTION value=\"\">OK</OPTION>"
				<< "<OPTION value=\"\">Warning</OPTION>"
				<< "<OPTION value=\"\">Warning+Error</OPTION>"
				<< "<OPTION value=\"\">Error</OPTION>"
				<< "</SELECT></td><td>Message</td><td>"
				<< "<SELECT name=\"Message\">"
				<< "<OPTION value=\"\" selected>(tous)</OPTION>"
				<< "<OPTION value=\"\">Un message</OPTION>"
				<< "<OPTION value=\"\">Conflit</OPTION>"
				<< "<OPTION value=\"\">Messages</OPTION>"
				<< "</SELECT></td></tr>"
				<< "<tr><td colspan=\"6\"><INPUT type=\"submit\" value=\"Rechercher\" /></td></tr></table>"
				<< "<p>Résultats de la recherche (tous) :</P>"
				<< "<TABLE><TR>"
				<< "<th>Sel</th><th>UID</th><th>Emplacement</th><th>Type</th><th>Etat</th><th>Msg</th>"
				<< "<th>Actions</th></tr>";

			// Loop on each resulting display screen

			{
				stream
					<< "<tr><td><INPUT type=\"checkbox\" name=\"Checkbox3\"></td>"
					<< "<td><A href=\"admin_display.htm\">5896215762324576</A></td>"
					<< "<td><A href=\"admin_display.htm\">TOULOUSE Matabiau/Marengo quai 1</A></td>"
					<< "<td><A href=\"admin_display.htm\">Oscar</A></td>"
					<< "<td></td>" // Bullets showing the states of the display
					<< "<td></td>" // Bullet showing the message status
					<< "<td><INPUT type=\"button\" value=\"Modifier\" name=\"Modifier\" />"
					<< "<INPUT type=\"button\" value=\"Simuler\" name=\"Modifier\" />"
					<< "<INPUT type=\"button\" value=\"Supervision\" name=\"Button1\" /></td>"
					<< "</tr>";
			}

			stream
				<< "<TR><TD>&nbsp;</TD><TD colSpan=\"4\">"
				<< "(sélectionner un afficheur existant pour copier ses&nbsp;propriétés dans le nouvel afficheur)"
				<< "</TD><TD></TD>"
				<< "<td>" << createDisplayRequest->getHTMLFormHeader("add")
				<< Html::getSubmitButton("Créer un nouvel afficheur")
				<< "</form></td>";

			delete createDisplayRequest;
		}
	}
}
