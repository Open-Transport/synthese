
/** MessagesScenarioAdmin class implementation.
	@file MessagesScenarioAdmin.cpp

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

#include "17_messages/MessagesScenarioAdmin.h"
#include "17_messages/MessageAdmin.h"
#include "17_messages/Scenario.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/ScenarioNameUpdateAction.h"
#include "17_messages/DeleteAlarmAction.h"
#include "17_messages/NewMessageAction.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"
#include "32_admin/ResultHTMLTable.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace messages
	{
		MessagesScenarioAdmin::MessagesScenarioAdmin()
			: AdminInterfaceElement("messageslibrary", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _scenario(NULL)
		{}


		void MessagesScenarioAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			AdminRequest::ParametersMap::const_iterator it;
			it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw AdminParametersException("Scenario not specified");
			if (Conversion::ToLongLong(it->second) != Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
			{
				if (!MessagesModule::getScenarii().contains(Conversion::ToLongLong(it->second)))
					throw AdminParametersException("Specified scenario not found");

				_scenario = MessagesModule::getScenarii().get(Conversion::ToLongLong(it->second));

				if (!_scenario->getIsATemplate())
					setSuperior("messages");
			}
		}

		string MessagesScenarioAdmin::getTitle() const
		{
			return _scenario->getName();
		}

		void MessagesScenarioAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioAdmin>());
			updateRequest->setAction(Factory<Action>::create<ScenarioNameUpdateAction>());
			updateRequest->setObjectId(_scenario->getKey());

			AdminRequest* messRequest = Factory<Request>::create<AdminRequest>();
			messRequest->copy(request);
			messRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());

			AdminRequest* deleteRequest = Factory<Request>::create<AdminRequest>();
			deleteRequest->copy(request);
			deleteRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioAdmin>());
			deleteRequest->setAction(Factory<Action>::create<DeleteAlarmAction>());
			deleteRequest->setObjectId(_scenario->getKey());

			AdminRequest* addRequest = Factory<Request>::create<AdminRequest>();
			addRequest->copy(request);
			addRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
			addRequest->setAction(Factory<Action>::create<NewMessageAction>());
			addRequest->setParameter(NewMessageAction::PARAMETER_SCENARIO_ID, Conversion::ToString(_scenario->getKey()));
			addRequest->setParameter(NewMessageAction::PARAMETER_IS_TEMPLATE, Conversion::ToString(_scenario->getKey()));

			stream
				<< "<h1>Propriété</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<P>Nom : " << Html::getTextInput(ScenarioNameUpdateAction::PARAMETER_NAME, _scenario->getName()) << Html::getSubmitButton("Modifier") << "</P>"
				<< "</form>"

				<< "<h1>Messages</h1>";

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "Message"));
			h.push_back(make_pair(string(), "Emplacement"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(h, NULL, string(), true, addRequest, NewMessageAction::PARAMETER_IS_TEMPLATE);

			stream << t.open();

			for(Scenario::AlarmsSet::const_iterator it = _scenario->getAlarms().begin(); it != _scenario->getAlarms().end(); ++it)
			{
				Alarm* alarm = *it;
				messRequest->setObjectId(alarm->getKey());
				deleteRequest->setParameter(DeleteAlarmAction::PARAMETER_ALARM, Conversion::ToString(alarm->getKey()));
				stream << t.row();
				stream << t.col() << alarm->getShortMessage();
				stream << t.col() << ""; // Emplacement
				stream << t.col() << Html::getLinkButton(messRequest->getURL(), "Modifier")
					<< "&nbsp;" << Html::getLinkButton(deleteRequest->getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le message du scénario ?");
			}

			stream << t.row();
			stream << t.col(3) << "(sélectionnez un&nbsp;message existant pour créer une copie)";
			stream << t.col() << Html::getSubmitButton("Ajouter");
			stream << t.close();

			delete updateRequest;
			delete messRequest;
			delete deleteRequest;
			delete addRequest;
		}
	}
}
