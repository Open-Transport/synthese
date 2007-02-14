
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

		void UserAdmin::display(std::ostream& stream, const AdminRequest* request) const
		{
			stream
				<< "<table>"
				<< "<tr><th colSpan=\"2\">Connexion</th></tr>"
				<< "<TR><TD>Login</TD><TD><input value=\"" << _user->getLogin() << "\" type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Mot de passe</TD><TD><INPUT type=\"password\" value=\"\" name=\"Password1\"></TD></TR>"
				<< "<TR><TD>Mot de passe (vérification)</TD><TD><INPUT type=\"password\" value=\"\" name=\"Password1\"></TD></TR>"
				<< "<TR><th colSpan=\"2\">Coordonnées</th></TR>"
				<< "<TR><TD>Prénom</TD><TD><INPUT type=\"text\" value=\"" << _user->getSurname() << "\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Nom</TD><TD><INPUT type=\"text\" value=\"" << _user->getName() << "\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Adresse</TD><TD><TEXTAREA name=\"Textarea1\" rows=\"2\" cols=\"20\">" << _user->getAddress() << "</TEXTAREA></TD></TR>"
				<< "<TR><TD>Code postal</TD><TD><INPUT type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Ville</TD><TD><INPUT type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>E-mail</TD><TD><INPUT type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><th colSpan=\"2\">Droits</TD></TR>"
				<< "<TR><td>Connexion autorisée</td><TD><INPUT type=\"radio\" CHECKED value=\"Radio1\" name=\"RadioGroupc\">OUI	<INPUT type=\"radio\" value=\"Radio1\" name=\"RadioGroupc\">NON</TD></TR>"
				<< "<tr><td>Profil</td><td>" << Html::getSelectInput("", SecurityModule::getProfileLabels(), _user->getProfile()->getKey()) << "</td></tr>"
				
				<< "<tr><td style=\"text-align:center\" colSpan=\"2\">" << Html::getSubmitButton("Enregistrer les modifications") << "</td></tr>"
				<< "</table>";
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
