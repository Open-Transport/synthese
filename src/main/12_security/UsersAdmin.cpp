
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

		void UsersAdmin::display( std::ostream& stream, const ParametersVector& parameters, const void* rootObject /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			// Cast the request to the actual type
			AdminRequest* currentRequest = (AdminRequest*) request;
			
			// Request for search form
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
			
			// Request for add user action form
			AdminRequest* addUserRequest = Factory<Request>::create<AdminRequest>();
			addUserRequest->copy(request);
			addUserRequest->setPage(Factory<AdminInterfaceElement>::create<UsersAdmin>());
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
				<< "Login <input name=\"" << PARAM_SEARCH_LOGIN << "\" value=\"" << currentRequest->getStringParameter(PARAM_SEARCH_LOGIN, "") << "\" />, "
				<< "Nom :<input name=\"" << PARAM_SEARCH_NAME << "\" value=\"" << currentRequest->getStringParameter(PARAM_SEARCH_NAME, "") << "\" />, "
				<< "Profil : <SELECT name=\"" << PARAM_SEARCH_PROFILE_ID << "\">";


			for (Profile::Registry::const_iterator it = SecurityModule::getProfiles().begin(); it != SecurityModule::getProfiles().end(); ++it)
				stream << "<option value=\"" << it->first << "\"" << ((currentRequest->getLongLongParameter(PARAM_SEARCH_PROFILE_ID, 0) == it->first) ? " selected=\"1\" " : "") << ">" << it->second->getName() << "</option>";

			stream
				<< "</SELECT>&nbsp;&nbsp;"
				<< "<input type=\"submit\" value=\"Rechercher\">"
				<< "</form>";

			// Launch the users search
			vector<User*> users = UserTableSync::searchUsers(ServerModule::getSQLiteThread()
				, currentRequest->getStringParameter(PARAM_SEARCH_LOGIN, "")
				, currentRequest->getStringParameter(PARAM_SEARCH_NAME, "")
				, currentRequest->getIntParameter(PARAM_SEARCH_FIRST, 0)
				, currentRequest->getIntParameter(PARAM_SEARCH_NUMBER, 0)
			);

			stream << "<h1>Résultats de la recherche</h1>";

			if (users.size() == 0)
				stream << "Aucun utilisateur trouvé";


			stream << "<table><tr><th>Sel</th><th>Login</th><th>Nom</th><th>Profil</th><th>Action</th></tr>";

			for(vector<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
			{
				User* user = *it;

				stream
					<< "<tr>"
					<< "<td><input type=\"checkbox\" name=\"selection\"></TD>"
					<< "<td>" << userRequest->getHTMLLink(user->getLogin()) << "</td>"
					<< "<td>" << userRequest->getHTMLLink(user->getName()) << "</td>"
					<< "<td>" << user->getProfile()->getName() << "</td>"
					<< "<td></TD>"
					<< "</TR>";
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
			if (currentRequest->getIntParameter(PARAM_SEARCH_NUMBER, 0) > 0 && users.size() >= currentRequest->getIntParameter(PARAM_SEARCH_NUMBER, 0))
				stream << "<p style=\"text-align:right\">Utilisateurs&nbsp;suivants</p>";

			stream
//				<< "<p>Sélection : <input type=\"submit\" value=\"Supprimer\" name=\"" << actionRequest->getAction()->getFactoryKey() << "\" /></p>"
				<< "<p>Cliquer sur un titre de colonne pour trier le tableau.</p>";
//				<< "</form>";

			delete addUserRequest;
			delete searchRequest;
		}
	}
}
