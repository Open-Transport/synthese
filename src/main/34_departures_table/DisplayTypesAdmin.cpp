
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
#include "11_interfaces/Interface.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/ModuleAdmin.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayTypesAdmin.h"
#include "34_departures_table/CreateDisplayTypeAction.h"
#include "34_departures_table/UpdateDisplayTypeAction.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/DisplayTypeRemoveAction.h"
#include "34_departures_table/ArrivalDepartureTableRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace departurestable;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DisplayTypesAdmin>::FACTORY_KEY("2displaytypes");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayTypesAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DisplayTypesAdmin>::DEFAULT_TITLE("Types d'afficheurs");
	}

	namespace departurestable
	{
		void DisplayTypesAdmin::setFromParametersMap(const ParametersMap& map)
		{
		}

		void DisplayTypesAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Right
			bool writeRight(request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER));

			vector<shared_ptr<DisplayType> > searchResult(DisplayTypeTableSync::search(string()));

			ActionFunctionRequest<CreateDisplayTypeAction,AdminRequest> createRequest(request);
			createRequest.getFunction()->setPage<DisplayTypesAdmin>();
			
			ActionFunctionRequest<UpdateDisplayTypeAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<DisplayTypesAdmin>();

			ActionFunctionRequest<DisplayTypeRemoveAction,AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage<DisplayTypesAdmin>();
			
			stream
				<< "<h1>Liste des types d'afficheurs disponibles</h1>"
				<< "<table id=\"searchresult\"><tr><th>Nom</th><th>Interface</th><th>Lignes</th><th>Max arrêts intermédiaires</th>";
			if (writeRight)
				stream << "<th>Actions</th>";
			stream << "</tr>";

			// Display types loop
			for (vector<shared_ptr<DisplayType> >::const_iterator it = searchResult.begin(); it != searchResult.end(); ++it)
			{
				shared_ptr<const DisplayType> dt = *it;
				deleteRequest.getAction()->setType(dt);

				HTMLForm uf(updateRequest.getHTMLForm("update" + Conversion::ToString(dt->getKey())));
				uf.setUpdateRight(writeRight);
				uf.addHiddenField(UpdateDisplayTypeAction::PARAMETER_ID, Conversion::ToString(dt->getKey()));
				stream << uf.open();
				stream
					<< "<tr>"
					<< "<td>" << uf.getTextInput(UpdateDisplayTypeAction::PARAMETER_NAME, dt->getName()) << "</td>"
					<< "<td>" << uf.getSelectInput(UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(), dt->getInterface()->getKey()) << "</td>"
					<< "<td>" << uf.getSelectNumberInput(UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99, dt->getRowNumber()) << "</td>"
					<< "<td>" << uf.getSelectNumberInput(UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER, UNKNOWN_VALUE, 99, dt->getMaxStopsNumber()) << "</td>";
					;
				if (writeRight)
					stream
						<< "<td>" << uf.getSubmitButton("Modifier") << "</td>"
						<< "<td>" << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le type " + dt->getName() + " ?", "monitor_delete.png");
				stream << "</tr>";
				stream << uf.close();
			}

			// New type
			if (writeRight)
			{
				HTMLForm cf(createRequest.getHTMLForm("create"));
				stream << cf.open();
				stream
					<< "<tr>"
					<< "<td>" << cf.getTextInput(CreateDisplayTypeAction::PARAMETER_NAME, "", "(Entrez le nom ici)") << "</td>"
					<< "<td>" << cf.getSelectInput(CreateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(), (uid) 0) << "</td>"
					<< "<td>" << cf.getSelectNumberInput(CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99) << "</td>"
					<< "<td>" << cf.getSelectNumberInput(CreateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER, UNKNOWN_VALUE, 99) << "</td>"
					<< "<td>" << cf.getSubmitButton("Ajouter") << "</td>"
					<< "</tr>";
				stream << cf.close();
			}

			stream << "</table>";
		}

		bool DisplayTypesAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER);
		}

		DisplayTypesAdmin::DisplayTypesAdmin()
			: AdminInterfaceElementTemplate<DisplayTypesAdmin>()
		{

		}

		AdminInterfaceElement::PageLinks DisplayTypesAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == DeparturesTableModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}

		bool DisplayTypesAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}
	}
}
