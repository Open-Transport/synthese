
/** DisplayTypesAdmin class implementation.
	@file DisplayTypesAdmin.cpp

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

#include "11_interfaces/InterfaceModule.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayTypesAdmin.h"
#include "34_departures_table/CreateDisplayTypeAction.h"
#include "34_departures_table/UpdateDisplayTypeAction.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace departurestable
	{
		/// @todo Verify the parent constructor parameters
		DisplayTypesAdmin::DisplayTypesAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::EVER_DISPLAYED) {}

		void DisplayTypesAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string DisplayTypesAdmin::getTitle() const
		{
			return "Types d'afficheurs";
		}

		void DisplayTypesAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* createRequest = Factory<Request>::create<AdminRequest>();
			createRequest->copy(request);
			createRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayTypesAdmin>());
			createRequest->setAction(Factory<Action>::create<CreateDisplayTypeAction>());

			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayTypesAdmin>());
			updateRequest->setAction(Factory<Action>::create<UpdateDisplayTypeAction>());

			stream
				<< "<h1>Liste des types d'afficheurs disponibles</h1>"
				<< "<table id=\"searchresult\"><tr><th>Nom</th><th>Interface</th><th>Lignes</th><th>Actions</th></tr>";

			// Display types loop
			for (DisplayType::Registry::const_iterator it = DeparturesTableModule::getDisplayTypes().begin(); it != DeparturesTableModule::getDisplayTypes().end(); ++it)
			{
				DisplayType* dt = it->second;

				stream
					<< updateRequest->getHTMLFormHeader("update" + Conversion::ToString(it->second->getKey()))
					<< "<tr>"
					<< "<td>" << Html::getTextInput(UpdateDisplayTypeAction::PARAMETER_NAME, dt->getName()) << "</td>"
					<< "<td>" << Html::getSelectInput(UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(), dt->getInterface()->getKey()) << "</td>"
					<< "<td>" << Html::getSelectNumberInput(UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99, dt->getRowNumber()) << "</td>"
					<< "<td>" << Html::getHiddenInput(UpdateDisplayTypeAction::PARAMETER_ID, Conversion::ToString(dt->getKey())) << Html::getSubmitButton("Modifier") << "</td>"
					<< "</tr></form>"
					;
			}

			// New type
			stream
				<< createRequest->getHTMLFormHeader("create")
				<< "<tr>"
				<< "<td>" << Html::getTextInput(CreateDisplayTypeAction::PARAMETER_NAME, "", "(Entrez le nom ici)") << "</td>"
				<< "<td>" << Html::getSelectInput(CreateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(), (uid) 0) << "</td>"
				<< "<td>" << Html::getSelectNumberInput(CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99) << "</td>"
				<< "<td>" << Html::getSubmitButton("Ajouter") << "</td>"
				<< "</tr></form>"
				<< "</table>";
		}
	}
}
