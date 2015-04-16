
/** UsersAdmin class implementation.
	@file UsersAdmin.cpp

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

// At first to avoid the Windows bug "WinSock.h has already been included"
#include "ServerModule.h"

#include "UserAdmin.h"

#include "HTMLModule.h"
#include "Profile.h"
#include "SearchFormHTMLTable.h"
#include "Session.h"
#include "StaticActionFunctionRequest.h"
#include "SecurityModule.h"
#include "AddUserAction.h"
#include "RemoveObjectAction.hpp"
#include "ProfileTableSync.h"
#include "UserTableSync.h"
#include "UsersAdmin.h"
#include "SecurityRight.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
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
	using namespace admin;
	using namespace html;
	using namespace security;
	using namespace db;

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
			const ParametersMap& map
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
			_requestParameters.setFromParametersMap(map, PARAM_SEARCH_LOGIN, 30);
		}



		util::ParametersMap UsersAdmin::getParametersMap() const
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
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<SecurityRight>(READ);
		}



		void UsersAdmin::display(
			std::ostream& stream,
			const server::Request& _request
		) const	{
			// Request for search form
			AdminFunctionRequest<UsersAdmin> searchRequest(_request, *this);
			SearchFormHTMLTable searchTable(searchRequest.getHTMLForm("search"));

			// Request for add user action form
			AdminActionFunctionRequest<AddUserAction, UserAdmin> addUserRequest(_request);
			addUserRequest.setActionFailedPage<UsersAdmin>();
			addUserRequest.setActionWillCreateObject();

			// Request for delete action form
			AdminActionFunctionRequest<RemoveObjectAction, UsersAdmin> deleteUserRequest(_request, *this);

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
					optional<RegistryKeyType>(_searchProfile.get() ? _searchProfile->getKey() : 0)
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


			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAM_SEARCH_LOGIN, "Login"));
			v.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			v.push_back(make_pair(PARAM_SEARCH_PROFILE_ID, "Profil"));
			v.push_back(make_pair(string(), "Actions"));

			HTMLForm f(addUserRequest.getHTMLForm("add"));

			ResultHTMLTable t(
				v,
				searchRequest.getHTMLForm(),
				_requestParameters,
				users
			);

			stream << f.open();
			stream << t.open();

			BOOST_FOREACH(const boost::shared_ptr<User>& user, users)
			{
				userRequest.getPage()->setUser(user);
				deleteUserRequest.getAction()->setObjectId(user->getKey());

				stream << t.row();
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getLogin());
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getName());
				stream << t.col() << (user->getProfile() ? user->getProfile()->getName() : "Pas de profil");
				stream << t.col() << userRequest.getHTMLForm().getLinkButton("Ouvrir") << "&nbsp;"
					<< deleteUserRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr(e) de vouloir supprimer l'utilisateur " + user->getLogin() + " ?");
			}

			stream << t.row();
			stream << t.col() << f.getTextInput(AddUserAction::PARAMETER_LOGIN, "", "Entrez le login ici");
			stream << t.col() << f.getTextInput(AddUserAction::PARAMETER_NAME, "", "Entrez le nom ici");
			stream << t.col() << f.getSelectInput(
				AddUserAction::PARAMETER_PROFILE_ID,
				SecurityModule::getProfileLabels(),
				optional<RegistryKeyType>(0)
			);
			stream << t.col() << f.getSubmitButton("Ajouter");
			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks UsersAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const SecurityModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			return links;
		}


		AdminInterfaceElement::PageLinks UsersAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			const UserAdmin* ua(
				dynamic_cast<const UserAdmin*>(&currentPage)
			);

			if(ua)
			{
				boost::shared_ptr<UserAdmin> p(getNewPage<UserAdmin>());
				p->setUserC(ua->getUser());
				links.push_back(p);
			}

			return links;
		}

	}
}
