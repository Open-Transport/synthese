
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
#include "InterfaceModule.h"

#include "SecurityModule.h"
#include "UserAdmin.h"
#include "AddUserAction.h"
#include "DelUserAction.h"
#include "ProfileTableSync.h"
#include "UserTableSync.h"
#include "UsersAdmin.h"
#include "SecurityRight.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "Session.h"
#include "ActionFunctionRequest.h"

#include "AdminModule.h"
#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"

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
			bool objectWillBeCreatedLater
		){
			if(!map.getDefault<string>(PARAM_SEARCH_LOGIN).empty())
			{
				_searchLogin = map.getOptional<string>(PARAM_SEARCH_LOGIN);
			}
			if(!map.getDefault<string>(PARAM_SEARCH_NAME).empty())
			{
				_searchName = map.getOptional<string>(PARAM_SEARCH_NAME);
			}
			if(!map.getDefault<string>(PARAM_SEARCH_SURNAME).empty())
			{
				_searchSurname = map.getOptional<string>(PARAM_SEARCH_SURNAME);
			}

			// Searched profile
			if (map.getOptional<RegistryKeyType>(PARAM_SEARCH_PROFILE_ID)) try
			{
				_searchProfile = ProfileTableSync::Get(map.get<RegistryKeyType>(PARAM_SEARCH_PROFILE_ID), _getEnv());
			}
			catch(...)
			{
			}

			// Table Parameters
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_LOGIN, 30);
		}
		
		
		
		server::ParametersMap UsersAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_searchLogin)
				m.insert(PARAM_SEARCH_LOGIN, *_searchLogin);
			if(_searchName)
				m.insert(PARAM_SEARCH_NAME, *_searchName);
			if(_searchSurname)
				m.insert(PARAM_SEARCH_SURNAME, *_searchSurname);
			if(_searchProfile.get())
			{
				m.insert(PARAM_SEARCH_PROFILE_ID, _searchProfile->getKey());
			}
			return m;
		}



		bool UsersAdmin::isAuthorized(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{
			return _request.isAuthorized<SecurityRight>(READ);
		}



		void UsersAdmin::display(
			std::ostream& stream,
			interfaces::VariablesMap& variables,
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{
			// Request for search form
			AdminFunctionRequest<UsersAdmin> searchRequest(_request);
			SearchFormHTMLTable searchTable(searchRequest.getHTMLForm("search"));
			
			// Request for add user action form
			AdminActionFunctionRequest<AddUserAction, UserAdmin> addUserRequest(_request);
			addUserRequest.getFunction()->setActionFailedPage<UsersAdmin>();
			addUserRequest.setActionWillCreateObject();

			// Request for delete action form
			AdminActionFunctionRequest<DelUserAction, UsersAdmin> deleteUserRequest(_request);
			
			// Request for user link
			AdminFunctionRequest<UserAdmin> userRequest(_request);

			// Search form
			stream << "<h1>Recherche d'utilisateur</h1>";
				
			stream << searchTable.open();
			stream << searchTable.cell("Login", searchTable.getForm().getTextInput(PARAM_SEARCH_LOGIN, _searchLogin ? *_searchLogin : string()));
			stream << searchTable.cell("Nom", searchTable.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName ? *_searchName : string()));
			stream << searchTable.cell("Prénom", searchTable.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname ? *_searchSurname : string()));
			stream << searchTable.cell("Profil", searchTable.getForm().getSelectInput(
					PARAM_SEARCH_PROFILE_ID,
					SecurityModule::getProfileLabels(true),
					_searchProfile.get() ? _searchProfile->getKey() : RegistryKeyType(0)
			)	);
			stream << searchTable.close();

			stream << "<h1>Résultats de la recherche</h1>";

			// Search
			UserTableSync::SearchResult users(
				UserTableSync::Search(
					_getEnv(),
					_searchLogin ? "%"+ *_searchLogin+"%" : _searchLogin,
					_searchName ? "%"+*_searchName+"%" : _searchName,
					_searchSurname ? "%"+ *_searchSurname+"%" : _searchSurname,
					optional<string>(),
					_searchProfile.get() ? _searchProfile->getKey() : optional<RegistryKeyType>(),
					logic::indeterminate,
					logic::indeterminate,
					optional<RegistryKeyType>(),
					_requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAM_SEARCH_LOGIN
					, _requestParameters.orderField == PARAM_SEARCH_NAME
					, _requestParameters.orderField == PARAM_SEARCH_PROFILE_ID
					, _requestParameters.raisingOrder,
					UP_LINKS_LOAD_LEVEL
			)	);
			
			if (users.empty())
				stream << "Aucun utilisateur trouvé";


			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAM_SEARCH_LOGIN, "Login"));
			v.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			v.push_back(make_pair(PARAM_SEARCH_PROFILE_ID, "Profil"));
			v.push_back(make_pair(string(), "Actions"));
			
			ActionResultHTMLTable t(
				v,
				searchRequest.getHTMLForm(),
				_requestParameters,
				users,
				addUserRequest.getHTMLForm("add"),
				string(),
				InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE)
			);

			stream << t.open();

			BOOST_FOREACH(shared_ptr<User> user, users)
			{
				userRequest.getPage()->setUser(user);
				deleteUserRequest.getAction()->setUser(user);
				stream << t.row();
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getLogin());
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getName());
				stream << t.col() << user->getProfile()->getName();
				stream << t.col() << userRequest.getHTMLForm().getLinkButton("Ouvrir") << "&nbsp;"
					<< deleteUserRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr(e) de vouloir supprimer l'utilisateur " + user->getLogin() + " ?");
			}

			stream << t.row();
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_LOGIN, "", "Entrez le login ici");
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_NAME, "", "Entrez le nom ici");
			stream << t.col() << t.getActionForm().getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), uid(0));
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}



		AdminInterfaceElement::PageLinks UsersAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if (moduleKey == SecurityModule::FACTORY_KEY && isAuthorized(request))
			{
				AddToLinks(links, getNewPage());
			}
			return links;
		}
		
		
		AdminInterfaceElement::PageLinks UsersAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			const UserAdmin* ua(
				dynamic_cast<const UserAdmin*>(&currentPage)
			);
			
			if(ua)
			{
				shared_ptr<UserAdmin> p(getNewOtherPage<UserAdmin>());
				p->setUserC(ua->getUser());
				AddToLinks(links, p);
			}
			
			return links;
		}

	}
}
