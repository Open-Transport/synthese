
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

#include "05_html/SearchFormHTMLTable.h"
#include "05_html/ActionResultHTMLTable.h"
#include "05_html/HTMLModule.h"

#include "11_interfaces/InterfaceModule.h"

#include "12_security/SecurityModule.h"
#include "12_security/UserAdmin.h"
#include "12_security/User.h"
#include "12_security/AddUserAction.h"
#include "12_security/DelUserAction.h"
#include "12_security/Profile.h"
#include "12_security/UserTableSync.h"
#include "12_security/UsersAdmin.h"
#include "12_security/SecurityRight.h"

#include "30_server/Session.h"
#include "30_server/ServerModule.h"
#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace interfaces;
	using namespace admin;
	using namespace html;

	namespace security
	{
		const std::string UsersAdmin::PARAM_SEARCH_PROFILE_ID = "searchprofileid";
		const std::string UsersAdmin::PARAM_SEARCH_NAME = "searchname";
		const std::string UsersAdmin::PARAM_SEARCH_LOGIN = "searchlogin";
		const std::string UsersAdmin::PARAM_SEARCH_FIRST = "searchfirst";
		const std::string UsersAdmin::PARAM_SEARCH_NUMBER = "searchnumber";


		UsersAdmin::UsersAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED)
		{
		}

		std::string UsersAdmin::getTitle() const
		{
			return "Utilisateurs";
		}

		void UsersAdmin::display( std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Request for search form
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			SearchFormHTMLTable searchTable(searchRequest.getHTMLForm("search"));
			
			// Request for add user action form
			ActionFunctionRequest<AddUserAction, AdminRequest> addUserRequest(request);
			addUserRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());
			addUserRequest.getFunction()->setActionFailedPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			
			// Request for delete action form
			ActionFunctionRequest<DelUserAction, AdminRequest> deleteUserRequest(request);
			deleteUserRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			
			// Request for user link
			FunctionRequest<AdminRequest> userRequest(request);
			userRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());

			// Search form
			stream << "<h1>Recherche d'utilisateur</h1>";
				
			stream << searchTable.open();
			stream << searchTable.cell("Login", searchTable.getForm().getTextInput(PARAM_SEARCH_LOGIN, _searchLogin));
			stream << searchTable.cell("Nom", searchTable.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
			stream << searchTable.cell("Profil", searchTable.getForm().getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(true), _searchProfileId));
			stream << searchTable.close();

			stream << "<h1>Résultats de la recherche</h1>";

			if (_users.size() == 0)
				stream << "Aucun utilisateur trouvé";


			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAM_SEARCH_LOGIN, "Login"));
			v.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			v.push_back(make_pair(PARAM_SEARCH_PROFILE_ID, "Profil"));
			v.push_back(make_pair("", "Actions"));
			ActionResultHTMLTable t(v, searchTable.getForm(), "", true, addUserRequest.getHTMLForm("add"),"", InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();

			for(vector<User*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
			{
				User* user = *it;
				userRequest.setObjectId(user->getKey());
				deleteUserRequest.setObjectId(user->getKey());
				stream << t.row();
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getLogin());
				stream << t.col() << HTMLModule::getHTMLLink(userRequest.getURL(), user->getName());
				stream << t.col() << user->getProfile()->getName();
				stream << t.col() << userRequest.getHTMLForm().getLinkButton("Editer") << "&nbsp;"
					<< deleteUserRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr(e) de vouloir supprimer l\\'utilisateur " + user->getLogin() + " ?");
			}

			stream << t.row();
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_LOGIN, "", "Entrez le login ici");
			stream << t.col() << t.getActionForm().getTextInput(AddUserAction::PARAMETER_NAME, "", "Entrez le nom ici");
			stream << t.col() << t.getActionForm().getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), (uid) 0);
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}

		void UsersAdmin::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Searched login
			it = map.find(PARAM_SEARCH_LOGIN);
			if (it != map.end())
				_searchLogin = it->second;

			// Searched name
			it = map.find(PARAM_SEARCH_NAME);
			if (it != map.end())
				_searchName = it->second;

			// Searched name
			it = map.find(PARAM_SEARCH_PROFILE_ID);
			if (it != map.end())
				_searchProfileId = Conversion::ToLongLong(it->second);
			else
				_searchProfileId = 0;

			// First
			it = map.find(PARAM_SEARCH_FIRST);
			if (it != map.end())
				_first = Conversion::ToInt(it->second);
			else
				_first = 0;

			// Number
			it = map.find(PARAM_SEARCH_NUMBER);
			if (it != map.end())
				_number = Conversion::ToInt(it->second);
			else
				_number = 20;

			// Launch the users search
			_users = UserTableSync::search(
				_searchLogin
				, _searchName
				, _searchProfileId
				, false
				, _first
				, _number + 1
				);

			// Display of the "next users" button
			if (_users.size() > _number)
			{
				_nextButton = true;
				vector<User*>::iterator uit = _users.end() - 1;
				delete *uit;
				_users.erase(uit);
			}
		}

		UsersAdmin::~UsersAdmin()
		{
			for(vector<User*>::iterator it = _users.begin(); it != _users.end(); ++it)
				delete *it;
		}

		bool UsersAdmin::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return request->isAuthorized<SecurityRight>(Right::READ);
		}
	}
}
