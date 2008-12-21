
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

#include "HTMLForm.h"
#include "Interface.h"
#include "InterfaceModule.h"
#include "ActionFunctionRequest.h"
#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "DisplayType.h"
#include "DeparturesTableModule.h"
#include "DisplayTypesAdmin.h"
#include "CreateDisplayTypeAction.h"
#include "UpdateDisplayTypeAction.h"
#include "DisplayTypeTableSync.h"
#include "DisplayTypeRemoveAction.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayTypeAdmin.h"
#include "QueryString.h"

#include <boost/foreach.hpp>

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
			DisplayTypeTableSync::Search(_env, string(), 0, UNKNOWN_VALUE, true, true, UP_LINKS_LOAD_LEVEL);
		}

		void DisplayTypesAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Right
			bool writeRight(request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER));
			
			ActionFunctionRequest<CreateDisplayTypeAction,AdminRequest> createRequest(request);
			createRequest.getFunction()->setPage<DisplayTypesAdmin>();
			
			ActionFunctionRequest<DisplayTypeRemoveAction,AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage<DisplayTypesAdmin>();

			FunctionRequest<AdminRequest> openRequest(request);
			openRequest.getFunction()->setPage<DisplayTypeAdmin>();
			

			stream
				<< "<h1>Liste des types d'afficheurs disponibles</h1>"
				<< "<table class=\"adminresults\"><tr><th>Nom</th><th>Interface d'affichage</th><th>Rangées</th><th>Max arrêts intermédiaires</th>"
				<< "<th>Accès</th>";
			if (writeRight)
				stream << "<th>Actions</th>";
			stream << "</tr>";

			// Display types loop
			BOOST_FOREACH(shared_ptr<DisplayType> dt, _env.getRegistry<DisplayType>())
			{
				deleteRequest.getAction()->setType(dt);
				openRequest.setObjectId(dt->getKey());

//				uf.setUpdateRight(writeRight);
				stream
					<< "<tr>"
					<< "<td>" << dt->getName() << "</td>"
					<< "<td>" << ((dt->getDisplayInterface() == NULL) ? "" : dt->getDisplayInterface()->getName()) << "</td>"
					<< "<td>" << dt->getRowNumber() << "</td>"
					<< "<td>" << dt->getMaxStopsNumber() << "</td>"
					<< "<td>" << openRequest.getHTMLForm().getLinkButton("Modifier", string(), "monitor_edit.png") << "</td>"
					;
				if (writeRight)
					stream
						<< "<td>" << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le type " + dt->getName() + " ?", "monitor_delete.png");
				stream << "</tr>";
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
					<< "<td colspan=\"2\">" << cf.getSubmitButton("Ajouter") << "</td>"
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



		AdminInterfaceElement::PageLinks DisplayTypesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage , const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			Env env;
			DisplayTypeTableSync::Search(env, string(), 0, UNKNOWN_VALUE, true, true);
			AdminInterfaceElement::PageLinks links;
			BOOST_FOREACH(shared_ptr<DisplayType> displayType, env.getRegistry<DisplayType>())
			{
				PageLink link;
				link.factoryKey = DisplayTypeAdmin::FACTORY_KEY;
				link.icon = DisplayTypeAdmin::ICON;
				link.name = displayType->getName();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString(displayType->getKey());
				links.push_back(link);
			}
			return links;
		}
	}
}
