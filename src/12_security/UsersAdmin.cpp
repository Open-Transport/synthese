
/** UsersAdmin class implementation.
	@file UsersAdmin.cpp

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

// At first to avoid the Windows bug "WinSock.h has already been included"
#include "ServerModule.h"

#include "SearchFormHTMLTable.h"
#include "ActionResultHTMLTable.h"
#include "HTMLModule.h"
#include "RequestMissingParameterException.h"
#include "InterfaceModule.h"

#include "SecurityModule.h"
#include "UserAdmin.h"
#include "AddUserAction.h"
#include "DelUserAction.h"
#include "ProfileTableSync.h"
#include "UserTableSync.h"
#include "UsersAdmin.h"
#include "SecurityRight.h"
#include "Request.h"
#include "Session.h"
#include "ActionFunctionRequest.h"

#include "AdminModule.h"
#include "ModuleAdmin.h"
#include "AdminRequest.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace interfaces;
	using namespace admin;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,UsersAdmin>::FACTORY_KEY("users");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<UsersAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<UsersAdmin>::DEFAULT_TITLE("Utilisateurs");
	}
	
	namespace security
	{
		const std::string UsersAdmin::PARAM_SEARCH_PROFILE_ID = "sp";
		const std::string UsersAdmin::PARAM_SEARCH_NAME = "sn";
		const std::string UsersAdmin::PARAM_SEARCH_LOGIN = "sl";
		const std::string UsersAdmin::PARAM_SEARCH_SURNAME = "ss";


		void UsersAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			_searchLogin = map.getString(PARAM_SEARCH_LOGIN, false, FACTORY_KEY);
			_searchName = map.getString(PARAM_SEARCH_NAME, false, FACTORY_KEY);
			_searchSurname = map.getString(PARAM_SEARCH_SURNAME, false, FACTORY_KEY);

			// Searched profile
			uid id(map.getUid(PARAM_SEARCH_PROFILE_ID, false, FACTORY_KEY));
			if (id != UNKNOWN_VALUE && _env.getRegistry<Profile>().contains(id))
			{
				_searchProfile = ProfileTableSync::Get(id, _env);
			}

			// Table Parameters
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_LOGIN, 30);

			if(!doDisplayPreparationActions) return;
			
			// Search
			UserTableSync::Search(
				_env,
				"%"+_searchLogin+"%"
				, "%"+_searchName+"%"
				, "%"+_searchSurname+"%"
				, "%"
				, _searchProfile.get() ? _searchProfile->getKey() : UNKNOWN_VALUE
				, false
				, _requestParameters.first
				, _requestParameters.maxSize
				, _requestParameters.orderField == PARAM_SEARCH_LOGIN
				, _requestParameters.orderField == PARAM_SEARCH_NAME
				, _requestParameters.orderField == PARAM_SEARCH_PROFILE_ID
				, _requestParameters.raisingOrder,
				UP_LINKS_LOAD_LEVEL
			);
			_resultParameters.setFromResult(_requestParameters, _env.getEditableRegistry<User>());
		}
		
		
		
		server::ParametersMap UsersAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAM_SEARCH_LOGIN, _searchLogin);
			m.insert(PARAM_SEARCH_NAME, _searchName);
			m.insert(PARAM_SEARCH_SURNAME, _searchSurname);
			if(_searchProfile.get())
			{
				m.insert(PARAM_SEARCH_PROFILE_ID, _searchProfile->getKey());
			}
			return m;
		}



		bool UsersAdmin::isAuthorized() const
		{
			return _request->isAuthorized<SecurityRight>(READ);
		}



		void UsersAdmin::display( std::ostream& stream, interfaces::VariablesMap& variables
		) const	{
			// Request for search form
			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setSamePage(this);
			SearchFormHTMLTable searchTable(searchRequest.getHTMLForm("search"));
			
			// Request for add user action form
			ActionFunctionRequest<AddUserAction, AdminRequest> addUserRequest(_request);
			addUserRequest.getFunction()->setPage<UserAdmin>();
			addUserRequest.getFunction()->setActionFailedPage<UsersAdmin>();
			addUserRequest.setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);

			// Request for delete action form
			ActionFunctionRequest<DelUserAction, AdminRequest> deleteUserRequest(_request);
			deleteUserRequest.getFunction()->setSamePage(this);
			
			// Request for user link
			FunctionRequest<AdminRequest> userRequest(_request);
			userRequest.getFunction()->setPage<UserAdmin>();

			// Search form
			stream << "<h1>Recherche d'utilisateur</h1>";
				
			stream << searchTable.open();
			stream << searchTable.cell("Login", searchTable.getForm().getTextInput(PARAM_SEARCH_LOGIN, _searchLogin));
			stream << searchTable.cell("Nom", searchTable.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
			stream << searchTable.cell("Prénom", searchTable.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname));
			stream << searchTable.cell("Profil", searchTable.getForm().getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(true), _searchProfile.get() ? _searchProfile->getKey() : uid(0)));
			stream << searchTable.close();

			stream << "<h1>Résultats de la recherche</h1>";

			if (_env.getRegistry<User>().empty())
				stream << "Aucun utilisateur trouvé";


			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAM_SEARCH_LOGIN, "Login"));
			v.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			v.push_back(make_pair(PARAM_SEARCH_PROFILE_ID, "Profil"));
			v.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t(v, searchRequest.getHTMLForm(), _requestParameters, _resultParameters, addUserRequest.getHTMLForm("add"),"", InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();

			BOOST_FOREACH(shared_ptr<User> user, _env.getRegistry<User>())
			{
				userRequest.setObjectId(user->getKey());
				deleteUserRequest.setObjectId(user->getKey());
				stream << t.row();
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getLogin());
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getName());
				stream << t.col() << user->getProfile()->getName();
				stream << t.col() << userRequest.getHTMLForm().getLinkButton("Modifier") << "&nbsp;"
					<< deleteUserRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr(e) de vouloir supprimer l\\'utilisateur " + user->getLogin() + " ?");
			}

			stream << t.row();
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_LOGIN, "", "Entrez le login ici");
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_NAME, "", "Entrez le nom ici");
			stream << t.col() << t.getActionForm().getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), uid(0));
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}

		UsersAdmin::UsersAdmin()
			: AdminInterfaceElementTemplate<UsersAdmin>()
		{
		
		}

		AdminInterfaceElement::PageLinks UsersAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == SecurityModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}
	}
}
