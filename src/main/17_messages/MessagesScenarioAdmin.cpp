
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
#include "17_messages/Scenario.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/ScenarioNameUpdateAction.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

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
			try
			{
				AdminRequest::ParametersMap::const_iterator it;

				it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
				if (it == map.end())
					throw AdminParametersException("Scenario not specified");
				_scenario = MessagesModule::getScenarii().get(Conversion::ToLongLong(it->second));
			}
			catch (Scenario::RegistryKeyException e)
			{
				throw AdminParametersException("Specified scenario not found");
			}
		}

		string MessagesScenarioAdmin::getTitle() const
		{
			return _scenario->getName();
		}

		void MessagesScenarioAdmin::display(ostream& stream, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioAdmin>());
			updateRequest->setAction(Factory<Action>::create<ScenarioNameUpdateAction>());

			stream
				<< "<h1>Propriété</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<P>Nom : " << Html::getTextInput(ScenarioNameUpdateAction::PARAMETER_NAME, _scenario->getName()) << Html::getSubmitButton("Modifier") << "</P>"
				<< "</form>"

				<< "<h1>Messages</h1>"
				<< "<table>"
				<< "<tr><th>Sel</th><th>Message</th><th>Emplacement</th>Actions</th></tr>";

			for(Scenario::AlarmsSet::const_iterator it = _scenario->getAlarms().begin(); it != _scenario->getAlarms().end(); ++it)
			{
				Alarm* alarm = *it;
				stream
					<< "<tr>"
					<< "<td>" << "<INPUT id=\"Radio2\" type=\"radio\" value=\"Radio2\" name=\"RadioGroup\">" << "</td>"
					<< "<td>" << alarm->getShortMessage() << "</td>"
					<< "<td>TOULOUSE Matabiau</td>"
					<< "<td>" << Html::getSubmitButton("Modifier") << Html::getSubmitButton("Supprimer") << "</td>"
					<< "</tr>";
			}

			stream
				<< "<TR>"
				<< "<TD colSpan=\"3\">(sélectionnez un&nbsp;message existant pour créer une copie)</TD>"
				<< "<TD>" << Html::getSubmitButton("Ajouter") << "</TD>"
				<< "</tr>"
				<< "</TABLE>";

			delete updateRequest;
		}
	}
}
