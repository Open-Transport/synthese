
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

#include "05_html/HTMLForm.h"

#include "11_interfaces/InterfaceModule.h"

#include "30_server/ActionFunctionRequest.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayTypesAdmin.h"
#include "34_departures_table/CreateDisplayTypeAction.h"
#include "34_departures_table/UpdateDisplayTypeAction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;

	namespace departurestable
	{
		/// @todo Verify the parent constructor parameters
		DisplayTypesAdmin::DisplayTypesAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::EVER_DISPLAYED) {}

		void DisplayTypesAdmin::setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string DisplayTypesAdmin::getTitle() const
		{
			return "Types d'afficheurs";
		}

		void DisplayTypesAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<CreateDisplayTypeAction,AdminRequest> createRequest(request);
			createRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayTypesAdmin>());
			
			ActionFunctionRequest<UpdateDisplayTypeAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayTypesAdmin>());
			
			stream
				<< "<h1>Liste des types d'afficheurs disponibles</h1>"
				<< "<table id=\"searchresult\"><tr><th>Nom</th><th>Interface</th><th>Lignes</th><th>Actions</th></tr>";

			// Display types loop
			for (DisplayType::Registry::const_iterator it = DeparturesTableModule::getDisplayTypes().begin(); it != DeparturesTableModule::getDisplayTypes().end(); ++it)
			{
				shared_ptr<const DisplayType> dt = it->second;

				HTMLForm uf(updateRequest.getHTMLForm("update" + Conversion::ToString(it->second->getKey())));
				uf.addHiddenField(UpdateDisplayTypeAction::PARAMETER_ID, Conversion::ToString(dt->getKey()));
				stream << uf.open();
				stream
					<< "<tr>"
					<< "<td>" << uf.getTextInput(UpdateDisplayTypeAction::PARAMETER_NAME, dt->getName()) << "</td>"
					<< "<td>" << uf.getSelectInput(UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(), dt->getInterface()->getKey()) << "</td>"
					<< "<td>" << uf.getSelectNumberInput(UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99, dt->getRowNumber()) << "</td>"
					<< "<td>" << uf.getSubmitButton("Modifier") << "</td>"
					<< "</tr>"
					;
				stream << uf.close();
			}

			// New type
			HTMLForm cf(createRequest.getHTMLForm("create"));
			stream << cf.open();
			stream
				<< "<tr>"
				<< "<td>" << cf.getTextInput(CreateDisplayTypeAction::PARAMETER_NAME, "", "(Entrez le nom ici)") << "</td>"
				<< "<td>" << cf.getSelectInput(CreateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(), (uid) 0) << "</td>"
				<< "<td>" << cf.getSelectNumberInput(CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99) << "</td>"
				<< "<td>" << cf.getSubmitButton("Ajouter") << "</td>"
				<< "</tr>";
			stream << cf.close();
			stream << "</table>";
		}

		bool DisplayTypesAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}
	}
}
