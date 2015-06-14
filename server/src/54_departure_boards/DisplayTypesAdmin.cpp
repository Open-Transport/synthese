
/** DisplayTypesAdmin class implementation.
	@file DisplayTypesAdmin.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DisplayTypesAdmin.h"

#include "AdminInterfaceElement.h"
#include "HTMLForm.h"
#include "Profile.h"
#include "StaticActionFunctionRequest.h"
#include "User.h"
#include "ModuleAdmin.h"
#include "DisplayType.h"
#include "DeparturesTableModule.h"
#include "CreateDisplayTypeAction.h"
#include "UpdateDisplayTypeAction.h"
#include "DisplayTypeTableSync.h"
#include "RemoveObjectAction.hpp"
#include "ArrivalDepartureTableRight.h"
#include "DisplayTypeAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "SearchFormHTMLTable.h"
#include "ActionResultHTMLTable.h"
#include "Profile.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace departure_boards;
	using namespace security;
	using namespace db;

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



		void DisplayTypesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_requestParameters.setFromParametersMap(map, Name::FIELD.name, 20);
			if(!map.getDefault<string>(PARAMETER_NAME).empty())
			{
				_searchName = map.getOptional<string>(PARAMETER_NAME);
			}
		}



		ParametersMap DisplayTypesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_searchName) m.insert(PARAMETER_NAME, *_searchName);
			return m;
		}



		void DisplayTypesAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			// Right
			bool writeRight(_request.getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER));

			AdminFunctionRequest<DisplayTypesAdmin> searchRequest(_request, *this);

			AdminActionFunctionRequest<CreateDisplayTypeAction,DisplayTypeAdmin> createRequest(
				_request
			);
			createRequest.setActionWillCreateObject();
			createRequest.setActionFailedPage<DisplayTypesAdmin>();

			AdminActionFunctionRequest<RemoveObjectAction,DisplayTypesAdmin> deleteRequest(
				_request,
				*this
			);

			AdminFunctionRequest<DisplayTypeAdmin> openRequest(_request);

			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable f(searchRequest.getHTMLForm());
			stream << f.open();
			stream << f.cell("Nom", f.getForm().getTextInput(PARAMETER_NAME, _searchName ? *_searchName : string()));
			stream << f.close();

			stream << "<h1>Résultat de la recherche</h1>";

			DisplayTypeTableSync::SearchResult types(
				DisplayTypeTableSync::Search(
					Env::GetOfficialEnv(),
					_searchName ? optional<string>("%"+ *_searchName +"%") : _searchName,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == Name::FIELD.name,
					_requestParameters.orderField == RowsNumber::FIELD.name,
					_requestParameters.raisingOrder
			)	);

			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(Name::FIELD.name, "Nom"));
			v.push_back(make_pair(RowsNumber::FIELD.name, "Nombre de rangées"));
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
			BOOST_FOREACH(const boost::shared_ptr<DisplayType>& dt, types)
			{
				deleteRequest.getAction()->setObjectId(dt->getKey());
				openRequest.getPage()->setType(dt);

				stream << t.row();
				stream << t.col() << dt->get<Name>();
				stream << t.col() << dt->get<RowsNumber>();

				stream <<
					t.col() <<
					openRequest.getHTMLForm().getLinkButton("Ouvrir", string(), "/admin/img/monitor_edit.png")
				;
				if (writeRight)
				{
					stream <<
						t.col() <<
						HTMLModule::getLinkButton(
							deleteRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le type " + dt->get<Name>() + " ?",
							"/admin/img/monitor_delete.png"
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
					t.getActionForm().getSelectNumberInput(
						CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER,
						1, 99
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
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const DeparturesTableModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		bool DisplayTypesAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			return true;
		}



		AdminInterfaceElement::PageLinks DisplayTypesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {

			DisplayTypeTableSync::SearchResult types(
				DisplayTypeTableSync::Search(
					Env::GetOfficialEnv()
			)	);
			AdminInterfaceElement::PageLinks links;
			BOOST_FOREACH(const boost::shared_ptr<DisplayType>& displayType, types)
			{
				boost::shared_ptr<DisplayTypeAdmin> p(getNewPage<DisplayTypeAdmin>());
				p->setType(displayType);
				links.push_back(p);
			}
			return links;
		}
	}
}
