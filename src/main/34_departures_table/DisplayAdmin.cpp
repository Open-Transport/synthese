
/** DisplayAdmin class implementation.
	@file DisplayAdmin.cpp

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
#include <sstream>

#include "01_util/Html.h"

#include "15_env/ConnectionPlace.h"

#include "32_admin/AdminRequest.h"

#include "34_departures_table/DisplayAdmin.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/UpdateDisplayScreenAction.h"
#include "34_departures_table/DisplayScreenTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;

	namespace departurestable
	{
		DisplayAdmin::DisplayAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}


		std::string DisplayAdmin::getTitle() const
		{
			stringstream s;
			s << "Afficheur ";
			if (_displayScreen->getLocalization() != NULL)
				s << _displayScreen->getLocalization()->getConnectionPlace()->getFullName() << "/" << _displayScreen->getLocalization()->getName();
			if (_displayScreen->getLocalizationComment().size() > 0)
				s << "/" << _displayScreen->getLocalizationComment();
			return s.str();
		}

		void DisplayAdmin::display(std::ostream& stream, const server::Request* request /*= NULL*/ ) const
		{
			// Update request
			AdminRequest* updateDisplayRequest = Factory<Request>::create<AdminRequest>();
			updateDisplayRequest->copy(request);
			updateDisplayRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			updateDisplayRequest->setAction(Factory<Action>::create<UpdateDisplayScreenAction>());

			map<int, string> blinkingDelaysMap;
			blinkingDelaysMap.insert(make_pair(0, "Pas de clignotement"));
			blinkingDelaysMap.insert(make_pair(1, "1 minute avant disparition"));
			for (int i=2; i<6; ++i)
				blinkingDelaysMap.insert(make_pair(i, Conversion::ToString(i) + " minutes avant disparition"));

			map<bool, string> boolMap;
			boolMap.insert(make_pair(true, "OUI"));
			boolMap.insert(make_pair(false, "NON"));

			map<DeparturesTableDirection, string> directionMap;
			directionMap.insert(make_pair(DISPLAY_ARRIVALS, "Arrivées"));
			directionMap.insert(make_pair(DISPLAY_DEPARTURES, "Départs"));

			map<EndFilter, string> endFilterMap;
			endFilterMap.insert(make_pair(ENDS_ONLY, "Origines/Terminus seulement"));
			endFilterMap.insert(make_pair(WITH_PASSING, "Origines/Terminus et passages"));

			map<int, string> clearDelayMap;
			for (i=5; i>1; --i)
				clearDelayMap.insert(make_pair(i, Conversion::ToString(-i) + " minutes avant le départ"));
			clearDelayMap.insert(make_pair(-1, "1 minute avant le départ"));
			clearDelayMap.insert(make_pair(0, "heure du départ"));
			clearDelayMap.insert(make_pair(1, "1 minute après le départ"));
			for (i=2; i<6; ++i)
				clearDelayMap.insert(make_pair(i, Conversion::ToString(i) + " minutes après le départ"));


			stream
				<< "<h1>Emplacement</h1>"
				<< "<table><tr><td>Lieu logique</td>"
				<< "<td><select name=\"\">"; // Script on update reload the page

			// Logical stops list

			stream
				<< "</select></td></tr>"
				<< "<tr><td>Lieu physique</td>"
				<< "<td><select name=\"\">";

			// Broadcast points list

			stream
				<< "</select></td></tr>"
				<< "<tr><td>Complément de précision</td>"
				<< "<td>" << Html::getTextInput(UpdateDisplayScreenAction::PARAMETER_LOCALIZATION_COMMENT, _displayScreen->getLocalizationComment()) << "</td></tr>"
				<< "</table>"

				<< "<h1>Données techniques</h1>"
				<< updateDisplayRequest->getHTMLFormHeader("updateprops")
				<< "<table>"
				<< "<tr><td>Type d'afficheur</td>"
				<< "<td><select name=\"\">";

			// Display types list

			stream
				<< "</select></td></tr>"
				<< "<tr><td>Code de branchement</td>"
				<< "<td>" << Html::getSelectNumberInput(UpdateDisplayScreenAction::PARAMETER_WIRING_CODE, 0, 99, _displayScreen->getWiringCode()) << "</td></tr>"
				<< "<tr><td>UID</td><td>" << _displayScreen->getKey() << "</td></tr>"
				<< "</table>"

				<< "<h1>Apparence</h1>"
				<< "<table><tr><td>Titre</td><td><input type=\"text\" name=\"\" value=\"\" /></td></tr>"
				<< "<tr><td>Clignotement</td><td>" << Html::getSelectInput(UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY, blinkingDelaysMap, _displayScreen->getBlinkingDelay()) << "</td></tr>"
				<< "<tr><td>Affichage numéro de quai</td><td>" << Html::getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM, boolMap, _displayScreen->getTrackNumberDisplay()) << "</td></tr>"
				<< "<tr><td>Affichage numéro de service</td><td>" << Html::getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER, boolMap, _displayScreen->getServiceNumberDisplay()) << "</td></tr>"
				<< "</table>"

				<< "<h1>Contenu</h1>"
				<< "<table>"
				<< "<tr><td>Horaires</td><td>" << Html::getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL, directionMap, _displayScreen->getDirection()) << "</td></tr>"
				<< "<tr><td>Sélection</td><td>" << Html::getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER, endFilterMap, _displayScreen->getEndFilter()) << "</td></tr>"
				<< "<tr><td>Délai maximum d'affichage</td><td>" << Html::getTextInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY, Conversion::ToString(_displayScreen->getMaxDelay())) << " minutes</td></tr>"
				<< "<tr><td>Délai d'effacement</td><td>" << Html::getSelectInput(UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY, clearDelayMap, _displayScreen->getClearingDelay()) << "</td></tr>"
				<< "<tr><td>Sélection sur arrêt physique</td><td><table>"
;
			// Physical stops loop
			{
				stream
					<< "<tr>"
					<< "<td><INPUT id=\"Checkbox1\" type=\"checkbox\" CHECKED name=\"Checkbox1\"></TD>"
					<< "<TD><FONT size=\"2\">Marengo quai 1</FONT></TD>"
					<< "</tr>";
			}
			stream
				<< "<tr>"
				<< "<td><INPUT id=\"Checkbox4\" type=\"checkbox\" name=\"Checkbox1\"></td>"
				<< "<td>Tous (y compris nouveaux)</td>"
				<< "</tr></table></td></tr>"
				<< "<tr><td>Sélection sur arrêt logique à ne pas desservir</td>"
				<< "<td><table>";

			// loop
			{
				stream
					<< "<tr><td>TOULOUSE Izards</td><td>" << Html::getSubmitButton("Supprimer") << "</td></tr>";
			}
			stream
				<< "<tr><td>" /* <SELECT id=\"Select9\" name=\"Origines seulement\">
														<OPTION value=\"\" selected>TOULOUSE Amat Massot</OPTION>
														<OPTION value=\"\">TOULOUSE LEP Bayard</OPTION>
													</SELECT> */
				<< "</td><td>" << Html::getSubmitButton("Ajouter") << "</td></tr>"
				<< "</table></td></tr></table>"

				<< "<h1>Présélection</h1>"

				<< "<table>"
				<< "<tr><td>Activer</td>"
				<< "<td>" << Html::getRadioInput(UpdateDisplayScreenAction::PARAMETER_ACTIVATE_PRESELECTION, boolMap, _displayScreen->getGenerationMethod() == DisplayScreen::STANDARD_METHOD) << "</td></tr>"
				<< "<tr><td>Délai maximum présélection</td><td>" << Html::getTextInput(UpdateDisplayScreenAction::PARAMETER_PRESELECTION_DELAY, Conversion::ToString(_displayScreen->getForceDestinationDelay())) << "</td></tr>"
				<< "<tr><td>Arrêts de desserte intermédiaire supplémentaires (les terminus des lignes sont automatiquement présélectionnés)</td>"
				<< "<td><table>";

			{
				stream
					<< "<tr><td>TOULOUSE Colombette</td><td>" << Html::getSubmitButton("Supprimer") << "</td></tr>";
			}

			stream
				<< "<tr><td>"
				/*	<SELECT id=\"Select10\" name=\"Origines seulement\">
														<OPTION value=\"\" selected>TOULOUSE Guilhemery</OPTION>
														<OPTION value=\"\">TOULOUSE LEP Bayard</OPTION>
													</SELECT> */
				<< "</td><td>" << Html::getSubmitButton("Ajouter") << "</td></tr>"
				<< "</table></td></tr>"

				<< "<tr><td colspan=\"2\">" << Html::getSubmitButton("Enregistrer les modifications") << "</td></tr>"
				<< "</table>"

				<< "<p>NB : Certains types d'afficheurs ne prennent pas en charge toutes les fonctionnalités proposées. Selon le type de l'afficheur, certains champs peuvent donc être sans effet sur l'affichage.</p>"
				;

			// Cleaning
			delete updateDisplayRequest;
		}

		void DisplayAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map .find(Request::PARAMETER_OBJECT_ID);
			if (it != map.end())
				_displayScreen = DisplayScreenTableSync::get(Conversion::ToLongLong(it->second));
		}
	}
}
