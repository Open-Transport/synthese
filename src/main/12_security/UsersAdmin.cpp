
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

#include "01_util/Html.h"

#include "11_interfaces/InterfaceModule.h"

#include "12_security/SecurityModule.h"
#include "12_security/UserAdmin.h"
#include "12_security/User.h"
#include "12_security/AddUserAction.h"
#include "12_security/DelUserAction.h"
#include "12_security/Profile.h"
#include "12_security/UserTableSync.h"
#include "12_security/UsersAdmin.h"

#include "30_server/ServerModule.h"

#include "32_admin/SearchFormHTMLTable.h"
#include "32_admin/ResultHTMLTable.h"
#include "32_admin/AdminModule.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace interfaces;
	using namespace admin;

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

		void UsersAdmin::display( std::ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			// Request for search form
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			SearchFormHTMLTable searchTable(searchRequest);
			
			// Request for add user action form
			AdminRequest* addUserRequest = Factory<Request>::create<AdminRequest>();
			addUserRequest->copy(request);
			addUserRequest->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());
			addUserRequest->setActionFailedPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			addUserRequest->setAction(Factory<Action>::create<AddUserAction>());

			// Request for delete action form
			AdminRequest* deleteUserRequest = Factory<Request>::create<AdminRequest>();
			deleteUserRequest->copy(request);
			deleteUserRequest->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			deleteUserRequest->setAction(Factory<Action>::create<DelUserAction>());

			// Request for user link
			AdminRequest* userRequest = Factory<Request>::create<AdminRequest>();
			userRequest->copy(request);
			userRequest->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());

			// Search form
			stream << "<h1>Recherche d'utilisateur</h1>";
				
			stream << searchTable.open();
			stream << searchTable.cell("Login", Html::getTextInput(PARAM_SEARCH_LOGIN, _searchLogin));
			stream << searchTable.cell("Nom", Html::getTextInput(PARAM_SEARCH_NAME, _searchName));
			stream << searchTable.cell("Profil", Html::getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(true), _searchProfileId));
			stream << searchTable.close();

			stream << "<h1>Résultats de la recherche</h1>";

			if (_users.size() == 0)
				stream << "Aucun utilisateur trouvé";


			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAM_SEARCH_LOGIN, "Login"));
			v.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			v.push_back(make_pair(PARAM_SEARCH_PROFILE_ID, "Profil"));
			v.push_back(make_pair("", "Actions"));
			ResultHTMLTable t(v, searchRequest, "", true, addUserRequest,"", InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();

			for(vector<User*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
			{
				User* user = *it;
				userRequest->setObjectId(user->getKey());
				deleteUserRequest->setObjectId(user->getKey());
				stream
					<< "<tr>"
					<< "<td>" << userRequest->getHTMLLink(user->getLogin()) << "</td>"
					<< "<td>" << userRequest->getHTMLLink(user->getName()) << "</td>"
					<< "<td>" << user->getProfile()->getName() << "</td>"
					<< "<td>" 
					<< Html::getLinkButton(userRequest->getURL(), "Editer") << "&nbsp;"
					<< Html::getLinkButton(deleteUserRequest->getURL(), "Supprimer", "Etes-vous sûr(e) de vouloir supprimer l\\'utilisateur " + user->getLogin() + " ?")
					<< "</td>"
					<< "</tr>";
			}

			stream
				<< "<tr>"
				<< "<td>" << Html::getTextInput(AddUserAction::PARAMETER_LOGIN, "", "Entrez le login ici") << "</td>"
				<< "<td>" << Html::getTextInput(AddUserAction::PARAMETER_NAME, "", "Entrez le nom ici") << "</td>"
				<< "<td>" << Html::getSelectInput(AddUserAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), (uid) 0) << "</td>"
				<< "<td>" << Html::getSubmitButton("Ajouter") << "</td>"
                << "</tr>"
				<< t.close();

			// If too much users
			if (_nextButton)
				stream << "<p style=\"text-align:right\">Utilisateurs&nbsp;suivants</p>";

			stream
				<< "<p>Cliquer sur un titre de colonne pour trier le tableau.</p>";

			delete addUserRequest;
			delete searchRequest;
			delete deleteUserRequest;
		}

		void UsersAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it;

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
	}
}
