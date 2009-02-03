
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
#include "SearchFormHTMLTable.h"
#include "ActionResultHTMLTable.h"

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
		const string DisplayTypesAdmin::PARAMETER_NAME("na");
		const string DisplayTypesAdmin::PARAMETER_INTERFACE_ID("ii");

		void DisplayTypesAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			_requestParameters.setFromParametersMap(map.getMap(), DisplayTypeTableSync::COL_NAME, 20);
			_searchName = map.getString(PARAMETER_NAME, false, FACTORY_KEY);
			_searchInterfaceId = map.getUid(PARAMETER_INTERFACE_ID, false, FACTORY_KEY);

			if(doDisplayPreparationActions)
			{
				DisplayTypeTableSync::Search(
					_env,
					"%"+ _searchName +"%",
					_searchInterfaceId,
					_requestParameters.first,
					_requestParameters.maxSize + 1,
					_requestParameters.orderField == DisplayTypeTableSync::COL_NAME,
					_requestParameters.orderField == DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID,
					_requestParameters.orderField == DisplayTypeTableSync::COL_ROWS_NUMBER,
					_requestParameters.raisingOrder,
					UP_LINKS_LOAD_LEVEL
				);
				_resultParameters.setFromResult(_requestParameters, _env.getEditableRegistry<DisplayType>());
			}
		}



		ParametersMap DisplayTypesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_NAME, _searchName);
			m.insert(PARAMETER_INTERFACE_ID, _searchInterfaceId);
			return m;
		}
		
			
			
		void DisplayTypesAdmin::display(
			ostream& stream,
			interfaces::VariablesMap& variables
		) const	{
			// Right
			bool writeRight(_request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER));
			
			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setSamePage(this);

			ActionFunctionRequest<CreateDisplayTypeAction,AdminRequest> createRequest(_request);
			createRequest.getFunction()->setSamePage(this);
			
			ActionFunctionRequest<DisplayTypeRemoveAction,AdminRequest> deleteRequest(_request);
			deleteRequest.getFunction()->setSamePage(this);

			FunctionRequest<AdminRequest> openRequest(_request);
			openRequest.getFunction()->setPage<DisplayTypeAdmin>();
			
			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable f(searchRequest.getHTMLForm());
			stream << f.open();
			stream << f.cell("Nom", f.getForm().getTextInput(PARAMETER_NAME, _searchName));
			stream << f.cell(
				"Interface d'affichage",
				f.getForm().getSelectInput(PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(true, true), _searchInterfaceId)
			);
			stream << f.close();

			stream << "<h1>Résultat de la recherche</h1>";

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
			ActionResultHTMLTable t(v, searchRequest.getHTMLForm(), _requestParameters, _resultParameters, createRequest.getHTMLForm("create"));

			// Display types loop
			stream << t.open();
			BOOST_FOREACH(shared_ptr<DisplayType> dt, _env.getRegistry<DisplayType>())
			{
				deleteRequest.getAction()->setType(dt);
				openRequest.setObjectId(dt->getKey());

				stream << t.row();
				stream << t.col() << dt->getName();
				stream << t.col() << ((dt->getDisplayInterface() == NULL) ? "(aucune)" : dt->getDisplayInterface()->getName());
				stream << t.col() << dt->getRowNumber();
				
				stream << t.col();
				if(	dt->getMonitoringInterface() != NULL &&
					dt->getTimeBetweenChecks() > 0
				){
					stream << dt->getMonitoringInterface()->getName();
				} else {
					stream << "(non supervisé)";
				}
				
				stream <<
					t.col() <<
					openRequest.getHTMLForm().getLinkButton("Modifier", string(), "monitor_edit.png")
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
				stream << t.col() << t.getActionForm().getTextInput(CreateDisplayTypeAction::PARAMETER_NAME, "", "(Entrez le nom ici)");
				stream << t.col() << t.getActionForm().getSelectInput(CreateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(true), RegistryKeyType(0));
				stream << t.col() << t.getActionForm().getSelectNumberInput(CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99);
				stream << t.col(2) << t.getActionForm().getSubmitButton("Ajouter");
			}
			stream << t.close();
		}

		bool DisplayTypesAdmin::isAuthorized() const
		{
			return _request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER);
		}

		DisplayTypesAdmin::DisplayTypesAdmin()
			: AdminInterfaceElementTemplate<DisplayTypesAdmin>(),
			_searchInterfaceId(UNKNOWN_VALUE)
		{

		}

		AdminInterfaceElement::PageLinks DisplayTypesAdmin::getSubPagesOfParent(
			const PageLink& parentLink,
			const AdminInterfaceElement& currentPage
		) const	{
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
			const AdminInterfaceElement& currentPage
		) const {
			Env env;
			DisplayTypeTableSync::Search(env, "%", UNKNOWN_VALUE, 0, UNKNOWN_VALUE, true, false, false, true, FIELDS_ONLY_LOAD_LEVEL);
			AdminInterfaceElement::PageLinks links;
			BOOST_FOREACH(shared_ptr<DisplayType> displayType, env.getRegistry<DisplayType>())
			{
				PageLink link(getPageLink());
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
