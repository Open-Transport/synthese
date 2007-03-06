
/** MessagesAdmin class implementation.
	@file MessagesAdmin.cpp

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
#include "15_env/CommercialLine.h"
#include "15_env/EnvModule.h"

#include "17_messages/Alarm.h"
#include "17_messages/MessagesAdmin.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessageAdmin.h"
#include "17_messages/NewMessageAction.h"
#include "17_messages/NewScenarioSendAction.h"
#include "17_messages/MessagesScenarioSendAdmin.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminParametersException.h"

#include "34_departures_table/DeparturesTableModule.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace departurestable;
	using namespace time;
	using namespace env;

	namespace messages
	{
		const std::string MessagesAdmin::PARAMETER_SEARCH_START = "mass";
		const std::string MessagesAdmin::PARAMETER_SEARCH_END = "mase";
		const std::string MessagesAdmin::PARAMETER_SEARCH_PLACE = "masp";
		const std::string MessagesAdmin::PARAMETER_SEARCH_LINE = "masl";

		MessagesAdmin::MessagesAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED)
			, _place(NULL), _line(NULL)
			, _startDate(TIME_UNKNOWN), _endDate(TIME_UNKNOWN)
		{}

		void MessagesAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			try
			{
				AdminRequest::ParametersMap::const_iterator it = map.find(PARAMETER_SEARCH_START);
				if (it != map.end())
				{
					_startDate.FromString(it->second);
				}

				it = map.find(PARAMETER_SEARCH_END);
				if (it != map.end())
				{
					_endDate.FromString(it->second);
				}

				it = map.find(PARAMETER_SEARCH_PLACE);
				if (it != map.end())
				{
					_place = EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(it->second));
				}

				it = map.find(PARAMETER_SEARCH_LINE);
				if (it != map.end())
				{
					_line = EnvModule::getCommercialLines().get(Conversion::ToLongLong(it->second));
				}

				_result = AlarmTableSync::search(_startDate, _endDate, _place, _line);
			}
			catch (ConnectionPlace::RegistryKeyException e)
			{
				throw AdminParametersException("Specified place not found ");
			}
			catch (CommercialLine::RegistryKeyException e)
			{
				throw AdminParametersException("Specified line not found ");
			}

		}

		string MessagesAdmin::getTitle() const
		{
			return "Messages";
		}

		void MessagesAdmin::display(ostream& stream, const AdminRequest* request) const
		{
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesAdmin>());

			AdminRequest* newMessageRequest = Factory<Request>::create<AdminRequest>();
			newMessageRequest->copy(request);
			newMessageRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
			newMessageRequest->setAction(Factory<Action>::create<NewMessageAction>());
			newMessageRequest->setActionFailedPage(Factory<AdminInterfaceElement>::create<MessagesAdmin>());

			AdminRequest* newScenarioRequest = Factory<Request>::create<AdminRequest>();
			newScenarioRequest->copy(request);
			newScenarioRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioSendAdmin>());
			newScenarioRequest->setAction(Factory<Action>::create<NewScenarioSendAction>());
			newScenarioRequest->setActionFailedPage(Factory<AdminInterfaceElement>::create<MessagesAdmin>());

			stream
				<< "<h1>Recherche</h1>"
				<< searchRequest->getHTMLFormHeader("search")
				<< "<table>"
				<< "<tr><td>Date début</td><td>" << Html::getTextInput(PARAMETER_SEARCH_START, _startDate.toString()) << "</td>"
				<< "<td>Date fin</td><td>" << Html::getTextInput(PARAMETER_SEARCH_END, _endDate.toString()) << "</td>"
				<< "<td>Zone d'arrêt</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_PLACE, DeparturesTableModule::getPlacesWithBroadcastPointsLabels(true), _place ? _place->getKey() : UNKNOWN_VALUE)	/// @todo put it in a factory to avoid dependency and to permit evolutivity
				<< "</td></tr>"
				<< "<tr><td>Ligne</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_LINE, EnvModule::getCommercialLineLabels(true), _line ? _line->getKey() : UNKNOWN_VALUE) << "</td>"
				<< "<td>Statut</td><td><SELECT name=\"Select1\">"
				<< "<OPTION value=\"\">(tous les états)</OPTION>"
				<< "<OPTION value=\"\">Diffusion terminée</OPTION>"
				<< "<OPTION value=\"\">En cours de diffusion</OPTION>"
				<< "<OPTION value=\"\">En cours avec date de fin</OPTION>"
				<< "<OPTION value=\"\">En cours sans date de fin</OPTION>"
				<< "<OPTION value=\"\">Diffusion ultérieure</OPTION>"
				<< "</SELECT></td><td>Conflit</td>"
				
				<< "<td><SELECT name=\"Select1\">"
				<< "<OPTION value=\"\">(toutes situations)</OPTION>"
				<< "<OPTION value=\"\">Sans conflits</OPTION>"
				<< "<OPTION value=\"\">Prioritaires sur complémentaires</OPTION>"
				<< "<OPTION value=\"\">En conflit</OPTION>"
				<< "</SELECT></td></tr>"
				
				<< "<tr><td>Type</td><td><SELECT name=\"Select1\">"
				<< "<OPTION value=\"\">(tous les types)</OPTION>"
				<< "<OPTION value=\"\">Prioritaire</OPTION>"
				<< "<OPTION value=\"\">Complémentaire</OPTION>"
				<< "</SELECT></td>"

				<< "<td colspan=\"4\">" << Html::getSubmitButton("Rechercher") << "</td></tr>"

				<< "</table></form>"

				<< "<h1>Résultats de la recherche</h1>"
				
				<< "<table>"
				<< "<tr><th>Sel</th><th>Dates</th><th>Message</th><th>Type</th><th>Etat</th><th>Confilt</th><th>Actions</th></tr>";

			for (vector<Alarm*>::const_iterator it= _result.begin(); it != _result.end(); ++it)
			{
				Alarm* alarm = *it;
				stream
					<< "<tr><td><INPUT type=\"radio\" value=\"Radio1\" name=\"RadioGroup\"></td>"
					<< "<td>dès le 2/9/2006</td>"
					<< "<td>" << alarm->getShortMessage() << "</td>"
					<< "<td>Complémentaire</td>"
					<< "<td></td>" // Bullet
					<< "<td></td>" // Bullet
					<< "<td>" << Html::getSubmitButton("Modifier")
					<< Html::getSubmitButton("Arrêter") << "</td>"
					<< "</tr>";
			}

			stream
				<< "</table>"
				<< "<P>(sélectionnez un message existant pour créer une copie)</P>"
				
				<< "<h1>Nouvelle diffusion</h1>"
				<< newMessageRequest->getHTMLFormHeader("newmess")
				<< "<p>" << Html::getSubmitButton("Nouvelle diffusion de message") << "</p>"
				<< "</form>"

				<< newScenarioRequest->getHTMLFormHeader("newsec")
				<< "<p>" << Html::getSubmitButton("Nouvelle diffusion de scénario") << "</p>"
				<< "<select name=\"Select1\">";

			// List of scenarios

			stream
				<< "</select></p>"
				<< "</form>"
				<< "<P align=\"right\">Messages&nbsp;suivants &gt;</P>"
				<< "<P>Cliquer sur un titre de colonne pour trier le tableau.</P>";

			delete searchRequest;
			delete newMessageRequest;
			delete newScenarioRequest;
		}
	}
}
