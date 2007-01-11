
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

#include "12_security/SecurityModule.h"
#include "12_security/UserAdmin.h"
#include "12_security/User.h"
#include "12_security/AddUserAction.h"
#include "12_security/Profile.h"
#include "12_security/UserTableSync.h"
#include "12_security/UsersAdmin.h"

#include "30_server/ServerModule.h"

#include "32_admin/AdminRequest.h"

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

		void UsersAdmin::display( std::ostream& stream, const Request* request) const
		{
			// Request for search form
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			
			// Request for add user action form
			AdminRequest* addUserRequest = Factory<Request>::create<AdminRequest>();
			addUserRequest->copy(request);
			addUserRequest->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());
			addUserRequest->setAction(Factory<Action>::create<AddUserAction>());

			// Request for delete action form
//			AdminRequest* deleteUserRequest = Factory<Request>::create<AdminRequest>();
//			deleteUserRequest->copy(request);
//			deleteUserRequest->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
//			deleteUserRequest->setAction(Factory<Action>::create<DelUserAction>());

			// Request for user link
			AdminRequest* userRequest = Factory<Request>::create<AdminRequest>();
			userRequest->copy(request);
			userRequest->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());

			// Search form
			stream
				<< searchRequest->getHTMLFormHeader("search")
				<< "<h1>Recherche d'utilisateur</h1>"
				<< "Login <input name=\"" << PARAM_SEARCH_LOGIN << "\" value=\"" << _searchLogin << "\" />, "
				<< "Nom :<input name=\"" << PARAM_SEARCH_NAME << "\" value=\"" << _searchName << "\" />, "
				<< "Profil : <SELECT name=\"" << PARAM_SEARCH_PROFILE_ID << "\">";


			stream << "<option value=\"\">(tous les profils)</option>";
			for (Profile::Registry::const_iterator it = SecurityModule::getProfiles().begin(); it != SecurityModule::getProfiles().end(); ++it)
				stream << "<option value=\"" << it->first << "\"" << ((_searchProfileId == it->first) ? " selected=\"1\" " : "") << ">" << it->second->getName() << "</option>";

			stream
				<< "</SELECT>&nbsp;&nbsp;"
				<< "<input type=\"submit\" value=\"Rechercher\">"
				<< "</form>";

			stream << "<h1>Résultats de la recherche</h1>";

			if (_users.size() == 0)
				stream << "Aucun utilisateur trouvé";


			stream << "<table><tr><th>Sel</th><th>Login</th><th>Nom</th><th>Profil</th><th>Action</th></tr>";

			for(vector<User*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
			{
				User* user = *it;
				userRequest->setObjectId(user->getKey());
				stream
					<< "<tr>"
					<< "<td><input type=\"checkbox\" name=\"selection\"></TD>"
					<< "<td>" << userRequest->getHTMLLink(user->getLogin()) << "</td>"
					<< "<td>" << userRequest->getHTMLLink(user->getName()) << "</td>"
					<< "<td>" << user->getProfile()->getName() << "</td>"
					<< "<td></td>"
					<< "</tr>";
			}

			stream
				<< addUserRequest->getHTMLFormHeader("adduser")
				<< "<tr>"
				<< "<td>&nbsp;</TD>"
				<< "<td><input value=\"Entrez le login ici\" name=\"" << AddUserAction::PARAMETER_LOGIN << "\" /></TD>"
				<< "<td><input value=\"Entrez le nom ici\" name=\"" << AddUserAction::PARAMETER_NAME << "\" /></TD>"
				<< "<td><SELECT name=\"" << AddUserAction::PARAMETER_PROFILE_ID << "\">";

			for (Profile::Registry::const_iterator it = SecurityModule::getProfiles().begin(); it != SecurityModule::getProfiles().end(); ++it)
				stream << "<option value=\"" << it->first << "\">" << it->second->getName() << "</option>";

			stream
				<< "</SELECT></TD>"
				<< "<td><input type=\"submit\" value=\"Ajouter\" /></td>"
                << "</tr></form>";

			stream
				<< "</TABLE>";

			// If too much users
			if (_nextButton)
				stream << "<p style=\"text-align:right\">Utilisateurs&nbsp;suivants</p>";

			stream
//				<< "<p>Sélection : <input type=\"submit\" value=\"Supprimer\" name=\"" << actionRequest->getAction()->getFactoryKey() << "\" /></p>"
				<< "<p>Cliquer sur un titre de colonne pour trier le tableau.</p>";
//				<< "</form>";

			delete addUserRequest;
			delete searchRequest;
		}

		void UsersAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
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
