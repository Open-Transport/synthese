
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
#include "StaticActionFunctionRequest.h"
#include "AdminInterfaceElement.h"
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
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "SearchFormHTMLTable.h"
#include "ActionResultHTMLTable.h"
#include "DeparturesTableInterfacePage.h"
#include "ParseDisplayReturnInterfacePage.h"
#include "InterfaceTableSync.h"
#include "Profile.h"

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
	using namespace departure_boards;
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

	namespace departure_boards
	{
		const string DisplayTypesAdmin::PARAMETER_NAME("na");
		const string DisplayTypesAdmin::PARAMETER_INTERFACE_ID("ii");



		void DisplayTypesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_requestParameters.setFromParametersMap(map.getMap(), DisplayTypeTableSync::COL_NAME, 20);
			if(!map.getDefault<string>(PARAMETER_NAME).empty())
			{
				_searchName = map.getOptional<string>(PARAMETER_NAME);
			}
			_searchInterfaceId = map.getOptional<RegistryKeyType>(PARAMETER_INTERFACE_ID);
		}



		ParametersMap DisplayTypesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_searchName) m.insert(PARAMETER_NAME, *_searchName);
			if(_searchInterfaceId) m.insert(PARAMETER_INTERFACE_ID, *_searchInterfaceId);
			return m;
		}
		
			
			
		void DisplayTypesAdmin::display(
			ostream& stream,
			interfaces::VariablesMap& variables,
			const admin::AdminRequest& _request
		) const	{
			// Right
			bool writeRight(_request.isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER));
			
			AdminFunctionRequest<DisplayTypesAdmin> searchRequest(_request);

			AdminActionFunctionRequest<CreateDisplayTypeAction,DisplayTypeAdmin> createRequest(
				_request
			);
			createRequest.setActionWillCreateObject();
			createRequest.getFunction()->setActionFailedPage<DisplayTypesAdmin>();
			
			AdminActionFunctionRequest<DisplayTypeRemoveAction,DisplayTypesAdmin> deleteRequest(
				_request
			);

			AdminFunctionRequest<DisplayTypeAdmin> openRequest(_request);
			
			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable f(searchRequest.getHTMLForm());
			stream << f.open();
			stream << f.cell("Nom", f.getForm().getTextInput(PARAMETER_NAME, _searchName ? *_searchName : string()));
			stream << f.cell(
				"Interface d'affichage",
				f.getForm().getSelectInput(
					PARAMETER_INTERFACE_ID,
					InterfaceTableSync::GetInterfaceLabels<DeparturesTableInterfacePage>(),
					_searchInterfaceId
				)
			);
			stream << f.close();

			stream << "<h1>Résultat de la recherche</h1>";

			DisplayTypeTableSync::SearchResult types(
				DisplayTypeTableSync::Search(
					_getEnv(),
					_searchName ? optional<string>("%"+ *_searchName +"%") : _searchName,
					_searchInterfaceId,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == DisplayTypeTableSync::COL_NAME,
					_requestParameters.orderField == DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID,
					_requestParameters.orderField == DisplayTypeTableSync::COL_ROWS_NUMBER,
					_requestParameters.raisingOrder,
					UP_LINKS_LOAD_LEVEL
			)	);
			
			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(DisplayTypeTableSync::COL_NAME, "Nom"));
			v.push_back(make_pair(DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID, "Interface d'affichage"));
			v.push_back(make_pair(DisplayTypeTableSync::COL_ROWS_NUMBER, "Nombre de rangées"));
			v.push_back(make_pair(DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID, "Protocole supervision"));
			v.push_back(make_pair(string(), "Actions"));
			if (writeRight)
			{
				v.push_back(make_pair(string(), "Actions"));
			}
			
			ActionResultHTMLTable t(
				v,
				searchRequest.getHTMLForm(),
				_requestParameters,
				types,
				createRequest.getHTMLForm("create")
			);

			// Display types loop
			stream << t.open();
			BOOST_FOREACH(shared_ptr<DisplayType> dt, types)
			{
				deleteRequest.getAction()->setType(dt);
				openRequest.getPage()->setType(dt);

				stream << t.row();
				stream << t.col() << dt->getName();
				stream << t.col() << ((dt->getDisplayInterface() == NULL) ? "(aucune)" : dt->getDisplayInterface()->getName());
				stream << t.col() << dt->getRowNumber();
				
				stream << t.col();
				if(	dt->getMonitoringInterface() != NULL &&
					dt->getTimeBetweenChecks().minutes() > 0
				){
					stream << dt->getMonitoringInterface()->getName();
				} else {
					stream << "(non supervisé)";
				}
				
				stream <<
					t.col() <<
					openRequest.getHTMLForm().getLinkButton("Ouvrir", string(), "monitor_edit.png")
				;
				if (writeRight)
				{
					stream <<
						t.col() <<
						HTMLModule::getLinkButton(
							deleteRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le type " + dt->getName() + " ?",
							"monitor_delete.png"
						)
					;
				}
			}

			// New type
			if (writeRight)
			{
				stream << t.row();
				
				stream <<
					t.col() <<
					t.getActionForm().getTextInput(
						CreateDisplayTypeAction::PARAMETER_NAME,
						"", "(Entrez le nom ici)"
					)
				;
				stream <<
					t.col() <<
					t.getActionForm().getSelectInput(
						CreateDisplayTypeAction::PARAMETER_INTERFACE_ID,
						InterfaceTableSync::GetInterfaceLabels<DeparturesTableInterfacePage>(optional<string>()),
						optional<RegistryKeyType>(0)
					)
				;
				stream <<
					t.col() <<
					t.getActionForm().getSelectNumberInput(
						CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER,
						1, 99
					)
				;
				stream <<
					t.col() <<
					t.getActionForm().getSelectInput(
						CreateDisplayTypeAction::PARAMETER_MONITORING_INTERFACE_ID,
						InterfaceTableSync::GetInterfaceLabels<ParseDisplayReturnInterfacePage>(optional<string>()),
						optional<RegistryKeyType>(0)
					)
				;
				stream << t.col(2) << t.getActionForm().getSubmitButton("Ajouter");
			}
			stream << t.close();
		}

		bool DisplayTypesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(
				READ,
				UNKNOWN_RIGHT_LEVEL,
				GLOBAL_PERIMETER
			);
		}

		
		
		AdminInterfaceElement::PageLinks DisplayTypesAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if (moduleKey == DeparturesTableModule::FACTORY_KEY && request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewPage());
			}
			return links;
		}



		bool DisplayTypesAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			return true;
		}



		AdminInterfaceElement::PageLinks DisplayTypesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const {
		
			DisplayTypeTableSync::SearchResult types(
				DisplayTypeTableSync::Search(
					*_env,
					optional<string>(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true,
					false,
					false,
					true,
					UP_LINKS_LOAD_LEVEL
			)	);
			AdminInterfaceElement::PageLinks links;
			BOOST_FOREACH(shared_ptr<DisplayType> displayType, types)
			{
				shared_ptr<DisplayTypeAdmin> p(getNewOtherPage<DisplayTypeAdmin>());
				p->setType(displayType);
				links.push_back(p);
			}
			return links;
		}
	}
}
