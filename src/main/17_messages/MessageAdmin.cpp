
/** MessageAdmin class implementation.
	@file MessageAdmin.cpp

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

#include "17_messages/MessageAdmin.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/Types.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/UpdateAlarmAction.h"
#include "17_messages/UpdateAlarmMessagesFromTemplateAction.h"
#include "17_messages/UpdateAlarmMessagesAction.h"
#include "17_messages/AlarmAddLinkAction.h"
#include "17_messages/AlarmRemoveLinkAction.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace messages
	{
		/// @todo Verify the parent constructor parameters
		MessageAdmin::MessageAdmin()
			: AdminInterfaceElement("messages", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}

		string MessageAdmin::getTitle() const
		{
			return _alarm->getShortMessage();
		}

		void MessageAdmin::setFromParametersMap( const AdminRequest::ParametersMap& map )
		{
			Request::ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw AdminParametersException("Missing message ID");

			if (Conversion::ToLongLong(it->second) == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			if (!MessagesModule::getAlarms().contains(Conversion::ToLongLong(it->second)))
				throw AdminParametersException("Invalid message ID");
			
			_alarm = MessagesModule::getAlarms().get(Conversion::ToLongLong(it->second));
		}

		void MessageAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
			updateRequest->setAction(Factory<Action>::create<UpdateAlarmAction>());
			updateRequest->setObjectId(request->getObjectId());

			stream
				<< "<h1>Paramètres</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Type</td><td>" << Html::getRadioInput(UpdateAlarmAction::PARAMETER_TYPE, MessagesModule::getLevelLabels(), _alarm->getLevel()) << "</td></tr>";
			if (_alarm->getScenario() == NULL)
			{
				stream
					<< "<tr><td>Début diffusion</td><td>Date " << Html::getTextInput(UpdateAlarmAction::PARAMETER_START_DATE, _alarm->getPeriodStart().getDate().toString()) 
					<< " Heure " << Html::getTextInput(UpdateAlarmAction::PARAMETER_START_HOUR, _alarm->getPeriodStart().getHour().toString()) << "</td></tr>"
					<< "<tr><td>Fin diffusion</td><td>Date " << Html::getTextInput(UpdateAlarmAction::PARAMETER_END_DATE, _alarm->getPeriodEnd().getDate().toString()) 
					<< " Heure " << Html::getTextInput(UpdateAlarmAction::PARAMETER_END_HOUR, _alarm->getPeriodEnd().getHour().toString()) << "</td></tr>"
					<< "<tr><td>Actif</td><td>" << Html::getOuiNonRadioInput(UpdateAlarmAction::PARAMETER_ENABLED, _alarm->getIsEnabled()) << "</td></tr>";
			}
			stream
				<< "<tr><td colspan=\"2\">" << Html::getSubmitButton("Enregistrer") << "</td></tr>"
				<< "</table></form>";

			delete updateRequest;

			if (_alarm->getLevel() != ALARM_LEVEL_UNKNOWN)
			{
				AdminRequest* templateRequest = Factory<Request>::create<AdminRequest>();
				templateRequest->copy(request);
				templateRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
				templateRequest->setAction(Factory<Action>::create<UpdateAlarmMessagesFromTemplateAction>());
				templateRequest->setObjectId(request->getObjectId());

				stream
					<< "<h1>Contenu</h1>"
					<< "<table>"
					<< "<tr><td>Modèle</td><td>" 
					<< templateRequest->getHTMLFormHeader("template")
					<< Html::getSelectInput(UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID, MessagesModule::getTextTemplateLabels(_alarm->getLevel()), uid()) 
					<< Html::getSubmitButton("Copier contenu")
					<< "</form></td></tr>";

				delete templateRequest;

				AdminRequest* updateMessagesRequest = Factory<Request>::create<AdminRequest>();
				updateMessagesRequest->copy(request);
				updateMessagesRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
				updateMessagesRequest->setAction(Factory<Action>::create<UpdateAlarmMessagesAction>());
				updateMessagesRequest->setObjectId(request->getObjectId());

				stream					
					<< updateMessagesRequest->getHTMLFormHeader("messages")
					<< "<tr><td>Message court</td><td>" << Html::getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE, _alarm->getShortMessage(), 2, 20) << "</td></tr>"
					<< "<tr><td>Message long</td><td>" << Html::getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE, _alarm->getLongMessage(), 4, 30) << "</td></tr>"
					<< "<tr><td colspan=\"2\">" << Html::getSubmitButton("Enregistrer") << "</td></tr>"
					<< "</form></table>";

				delete updateMessagesRequest;

				AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
				searchRequest->copy(request);
				searchRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
				searchRequest->setObjectId(request->getObjectId());

				AdminRequest* addRequest = Factory<Request>::create<AdminRequest>();
				addRequest->copy(request);
				addRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
				addRequest->setAction(Factory<Action>::create<AlarmAddLinkAction>());
				addRequest->setObjectId(request->getObjectId());
				addRequest->setParameter(AlarmAddLinkAction::PARAMETER_ALARM_ID, Conversion::ToString(_alarm->getKey()));

				AdminRequest* removeRequest = Factory<Request>::create<AdminRequest>();
				removeRequest->copy(request);
				removeRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
				removeRequest->setAction(Factory<Action>::create<AlarmRemoveLinkAction>());
				removeRequest->setObjectId(request->getObjectId());
				
				// Alarm messages destinations loop
				for (Factory<AlarmRecipient>::Iterator arit = Factory<AlarmRecipient>::begin(); arit != Factory<AlarmRecipient>::end(); ++arit)
				{
					addRequest->setParameter(AlarmAddLinkAction::PARAMETER_RECIPIENT_KEY, arit->getFactoryKey());
				
					stream << "<h1>Diffusion sur " << arit->getTitle() << "</h1>";

					arit->displayBroadcastListEditor(stream, _alarm, request->getParametersMap(), searchRequest, addRequest, removeRequest);
				}
				
				delete searchRequest;
				delete addRequest;
				delete removeRequest;
			}
		}
	}
}
