
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
		MessagesScenarioAdmin::MessagesScenarioAdmin()
			: AdminInterfaceElement("messageslibrary", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesScenarioAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string MessagesScenarioAdmin::getTitle() const
		{
			/// @todo Change the title of the page
			return "title";
		}

		void MessagesScenarioAdmin::display(ostream& stream, const AdminRequest* request) const
		{
			stream
				<< "<P>Nom : " << Html::getTextInput("", _scenario->getName()) << Html::getSubmitButton("Modifier") << "</P>"
				<< "<h1>Messages</h1>"
				<< "<table>"
				<< "<tr><th>Sel</th><th>Message</th><th>Emplacement</th>Actions</th></tr>";

			// Messages loop
			{
				stream
					<< "<TR>"
					<< "<TD><INPUT id=\"Radio2\" type=\"radio\" value=\"Radio2\" name=\"RadioGroup\"></TD>"
					<< "<TD>Le métro est interrompu...</TD>"
					<< "<TD>TOULOUSE Matabiau</TD>"
					<< "<td>" << Html::getSubmitButton("Modifier") << Html::getSubmitButton("Supprimer") << "</td>"
					<< "</TR>";
			}

			stream
				<< "<TR>"
				<< "<TD colSpan=\"3\">(sélectionnez un&nbsp;message existant pour créer une copie)</TD>"
				<< "<TD>" << Html::getSubmitButton("Ajouter") << "</TD>"
				<< "</tr>"
				<< "</TABLE>";

		}
	}
}
