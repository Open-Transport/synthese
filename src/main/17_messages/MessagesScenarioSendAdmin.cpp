
/** MessagesScenarioSendAdmin class implementation.
	@file MessagesScenarioSendAdmin.cpp

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


#include "17_messages/MessagesScenarioSendAdmin.h"
#include "17_messages/Scenario.h"
#include "17_messages/Alarm.h"
#include "17_messages/MessageAdmin.h"
#include "17_messages/ScenarioUpdateDatesAction.h"
#include "17_messages/DeleteAlarmAction.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
/*	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace messages
	{
		/// @todo Verify the parent constructor parameters
		MessagesScenarioSendAdmin::MessagesScenarioSendAdmin()
			: AdminInterfaceElement("superior", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesScenarioSendAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			AdminRequest::ParametersMap::const_iterator it;

			it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw AdminParametersException("Scenario not specified");

			/// @todo Initialize internal attributes from the map
		}

		string MessagesScenarioSendAdmin::getTitle() const
		{
			return _scenario ? _scenario->getName() : "Scénario";
		}

		void MessagesScenarioSendAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioSendAdmin>());
			updateRequest->setObjectId(request->getObjectId());
			updateRequest->setAction(Factory<Action>::create<ScenarioUpdateDatesAction>());

			AdminRequest* alarmRequest = Factory<Request>::create<AdminRequest>();
			alarmRequest->copy(request);
			alarmRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
			
			AdminRequest* deleteRequest = Factory<Request>::create<AdminRequest>();
			deleteRequest->copy(request);
			deleteRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioSendAdmin>());
			deleteRequest->setObjectId(request->getObjectId());
			deleteRequest->setAction(Factory<Action>::create<DeleteAlarmAction>());

			stream
				<< "<h1>Paramètres</h1>"
				<< updateRequest->getHTMLFormHeader("updatedate")
				<< "<table>"
				<< "<tr><td>Nom</td><td>" << Html::getTextInput(ScenarioUpdateDatesAction::PARAMETER_NAME, _scenario->getName())
				<< "<tr><td>Début diffusion</td><td>Date " << Html::getTextInput(ScenarioUpdateDatesAction::PARAMETER_START_DATE, _scenario->getPeriodStart().getDate().toString())
				<< "&nbsp;Heure " << Html::getTextInput(ScenarioUpdateDatesAction::PARAMETER_START_HOUR, _scenario->getPeriodStart().getHour().toString())
				<< "</td></tr>"
				<< "<tr><td>Fin diffusion</td><td><td>Date " << Html::getTextInput(ScenarioUpdateDatesAction::PARAMETER_END_DATE, _scenario->getPeriodEnd().getDate().toString())
				<< "&nbsp;Heure " << Html::getTextInput(ScenarioUpdateDatesAction::PARAMETER_END_HOUR, _scenario->getPeriodEnd().getHour().toString())
				<< " " << Html::getSubmitButton("Appliquer")
				<< "</td></tr>"
				<< "</table></form>"

				<< "<h1>Contenu</h1>"
				<< "<table>"
				<< "<tr><th>Sel</th><th>Message</th><th>Emplacements</th><th>Etat</th><th>Conflit</th><th colspan=\"2\">Actions</th></tr>";

			for (Scenario::AlarmsSet::const_iterator it = _scenario->getAlarms().begin(); it != _scenario->getAlarms().end(); ++it)
			{
				Alarm* alarm = *it;
				alarmRequest->setObjectId(alarm->getKey());
				stream
					<< "<tr>"
					<< "<td>" << "<INPUT id=\"Radio2\" type=\"radio\" value=\"Radio2\" name=\"RadioGroup\">" << "</td>"
					<< "<td>" << alarm->getShortMessage() << "</td>"
					<< "<td>TOULOUSE Matabiau</td>"
					<< "<td></td>"
					<< "<td></td>"
					<< "<td>" 
					<< alarmRequest->getHTMLFormHeader("alarm" + Conversion::ToString(alarm->getKey()))
					<< Html::getSubmitButton("Modifier")
					<< "</form></td>"
					<< "<td>"
					<< deleteRequest->getHTMLFormHeader("delete" + Conversion::ToString(alarm->getKey()))
					<< Html::getHiddenInput(DeleteAlarmAction::PARAMETER_ALARM, Conversion::ToString(alarm->getKey()))
					<< Html::getSubmitButton("Supprimer")
					<< "</form></td>"
					<< "</tr>";
			}

			stream
				<< "<tr>"
				<< "<td colspan=\"5\">(sélectionnez un&nbsp;message existant pour créer une copie)</td>"
				<< "<td colspan=\"2\">" << Html::getSubmitButton("Ajouter") << "</td>"
				<< "</tr>"
				<< "</table>"

				<< "<h1>Diffusion</h1>";

			if (_scenario->getIsEnabled())
			{
				stream << Html::getSubmitButton("Arrêter");
			}

			if (!_scenario->getIsEnabled()
			&& _scenario->getAlarms().size()
			&& !_scenario->getPeriodStart().isUnknown()
			){
				stream << Html::getSubmitButton("Envoyer");
			}
		}
	}*/
}
