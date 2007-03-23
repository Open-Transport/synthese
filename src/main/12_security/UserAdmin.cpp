
/** UserAdmin class implementation.
	@file UserAdmin.cpp

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

#include "02_db/DBEmptyResultException.h"

#include "12_security/User.h"
#include "12_security/UserAdmin.h"
#include "12_security/UserTableSync.h"
#include "12_security/SecurityModule.h"
#include "12_security/UserUpdateAction.h"
#include "12_security/UserPasswordUpdateAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace db;

	namespace security
	{
		const string UserAdmin::PARAM_USER_ID = "roid";

		UserAdmin::UserAdmin()
			: AdminInterfaceElement("users", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _user(NULL), _userError(false)
		{
			
		}

		std::string UserAdmin::getTitle() const
		{
			return (_user != NULL)
				? _user->getSurname() + " " + _user->getName()
				: "";
		}

		void UserAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());
			updateRequest->setAction(Factory<Action>::create<UserUpdateAction>());
			updateRequest->setObjectId(request->getObjectId());

			AdminRequest* userPasswordUpdateRequest = Factory<Request>::create<AdminRequest>();
			userPasswordUpdateRequest->copy(request);
			userPasswordUpdateRequest->setPage(Factory<AdminInterfaceElement>::create<UserAdmin>());
			userPasswordUpdateRequest->setAction(Factory<Action>::create<UserPasswordUpdateAction>());
			userPasswordUpdateRequest->setObjectId(request->getObjectId());

			stream
				<< "<h1>Coordonnées</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><th colspan=\"2\">Connexion</th></tr>"
				<< "<tr><td>Login</td><td>" << Html::getTextInput(UserUpdateAction::PARAMETER_LOGIN, _user->getLogin()) << "</td></tr>"
				<< "<tr><th colspan=\"2\">Coordonnées</th></tr>"
				<< "<tr><td>Prénom</td><td>" << Html::getTextInput(UserUpdateAction::PARAMETER_SURNAME, _user->getSurname()) << "</td></tr>"
				<< "<tr><td>Nom</td><td>" << Html::getTextInput(UserUpdateAction::PARAMETER_NAME, _user->getName()) << "</td></tr>"
				<< "<tr><td>Adresse</td><td>" << Html::getTextAreaInput(UserUpdateAction::PARAMETER_ADDRESS, _user->getAddress(), 4, 50) << "</td></tr>"
				<< "<tr><td>Code postal</td><td>" << Html::getTextInput(UserUpdateAction::PARAMETER_POSTAL_CODE, _user->getPostCode()) << "</td></tr>"
				<< "<tr><td>Ville</td><td>" << Html::getTextInput(UserUpdateAction::PARAMETER_CITY, _user->getCityText()) << "</td></tr>"
				<< "<tr><td>Téléphone</td><td>" << Html::getTextInput(UserUpdateAction::PARAMETER_PHONE, _user->getPhone()) << "</td></tr>"
				<< "<tr><td>E-mail</td><td>" << Html::getTextInput(UserUpdateAction::PARAMETER_EMAIL, _user->getEMail()) << "</td></tr>"
				<< "<tr><th colSpan=\"2\">Droits</td></tr>"
				<< "<tr><td>Connexion autorisée</td><td>" << Html::getOuiNonRadioInput(UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN, _user->getConnectionAllowed()) << "</td></tr>"
				<< "<tr><td>Profil</td><td>" << Html::getSelectInput(UserUpdateAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), _user->getProfile()->getKey()) << "</td></tr>"
				<< "<tr><td style=\"text-align:center\" colSpan=\"2\">" << Html::getSubmitButton("Enregistrer les modifications") << "</td></tr>"
				<< "</table>"
				<< "</form>"

				<< "<h1>Changement de mot de passe</h1>"
				<< userPasswordUpdateRequest->getHTMLFormHeader("pass")
				<< "<table>"
				<< "<tr><td>Mot de passe</td><td>" << Html::getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS1, "") << "</td></tr>"
				<< "<tr><td>Mot de passe (vérification)</td><td>" << Html::getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS2, "") << "</td></tr>"
				<< "<tr><td style=\"text-align:center\" colSpan=\"2\">" << Html::getSubmitButton("Changer le mot de passe") << "</td></tr>"
				<< "</table></form>";

			delete updateRequest;
			delete userPasswordUpdateRequest;
		}

		void UserAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::const_iterator it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
				if (it != map.end())
					_user = UserTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (DBEmptyResultException& e)
			{
				// throw AdminElementInitializationException("Bad user");
			}
		}

		UserAdmin::~UserAdmin()
		{
			delete _user;
		}
	}
}
