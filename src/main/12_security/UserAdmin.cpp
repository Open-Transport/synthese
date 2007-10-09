
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

#include "05_html/HTMLTable.h"
#include "05_html/HTMLForm.h"

#include "02_db/DBEmptyResultException.h"

#include "12_security/UserAdmin.h"
#include "12_security/UsersAdmin.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/SecurityModule.h"
#include "12_security/UserUpdateAction.h"
#include "12_security/UserPasswordUpdateAction.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace db;
	using namespace html;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, UserAdmin>::FACTORY_KEY("user");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<UserAdmin>::ICON("user.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<UserAdmin>::DISPLAY_MODE(AdminInterfaceElement::DISPLAYED_IF_CURRENT);
		template<> string AdminInterfaceElementTemplate<UserAdmin>::getSuperior()
		{
			return UsersAdmin::FACTORY_KEY;
		}
	}
	
	namespace security
	{
		const string UserAdmin::PARAM_USER_ID = "roid";

		UserAdmin::UserAdmin()
			: AdminInterfaceElementTemplate<UserAdmin>()
			, _userError(false)
		{
		}

		std::string UserAdmin::getTitle() const
		{
			return (_user.get())
				? _user->getSurname() + " " + _user->getName()
				: "";
		}

		void UserAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<UserUpdateAction, AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<UserAdmin>();
			updateRequest.setObjectId(request->getObjectId());

			ActionFunctionRequest<UserPasswordUpdateAction, AdminRequest> userPasswordUpdateRequest(request);
			userPasswordUpdateRequest.getFunction()->setPage<UserAdmin>();
			userPasswordUpdateRequest.setObjectId(request->getObjectId());

			{
				stream << "<h1>Coordonnées</h1>";
				HTMLForm form(updateRequest.getHTMLForm("update"));
				stream << form.open();
				HTMLTable t;
				stream << t.open();
				stream << t.row();
				stream << t.col(2) << "Connexion";
				stream << t.row();
				stream << t.col() << "Login";
				stream << t.col() << form.getTextInput(UserUpdateAction::PARAMETER_LOGIN, _user->getLogin());
				stream << t.row();
				stream << t.col(2) << "Coordonnées";
				stream << t.row();
				stream << t.col() << "Prénom";
				stream << t.col() << form.getTextInput(UserUpdateAction::PARAMETER_SURNAME, _user->getSurname());
				stream << t.row();
				stream << t.col() << "Nom";
				stream << t.col() << form.getTextInput(UserUpdateAction::PARAMETER_NAME, _user->getName());
				stream << t.row();
				stream << t.col() << "Adresse";
				stream << t.col() << form.getTextAreaInput(UserUpdateAction::PARAMETER_ADDRESS, _user->getAddress(), 4, 50);
				stream << t.row();
				stream << t.col() << "Code postal";
				stream << t.col() << form.getTextInput(UserUpdateAction::PARAMETER_POSTAL_CODE, _user->getPostCode());
				stream << t.row();
				stream << t.col() << "Ville";
				stream << t.col() << form.getTextInput(UserUpdateAction::PARAMETER_CITY, _user->getCityText());
				stream << t.row();
				stream << t.col() << "Téléphone";
				stream << t.col() << form.getTextInput(UserUpdateAction::PARAMETER_PHONE, _user->getPhone());
				stream << t.row();
				stream << t.col() << "E-mail";
				stream << t.col() << form.getTextInput(UserUpdateAction::PARAMETER_EMAIL, _user->getEMail());
				stream << t.row();
				stream << t.col(2) << "Droits";
				stream << t.row();
				stream << t.col() << "Connexion autorisée";
				stream << t.col() << form.getOuiNonRadioInput(UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN, _user->getConnectionAllowed());
				stream << t.row();
				stream << t.col() << "Profil";
				stream << t.col() << form.getSelectInput(UserUpdateAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), _user->getProfile()->getKey());
				stream << t.row();
				stream << t.col(2) << form.getSubmitButton("Enregistrer les modifications");
				stream << t.close();
				stream << form.close();
			}

			stream << "<h1>Changement de mot de passe</h1>";
			{
				HTMLForm form(userPasswordUpdateRequest.getHTMLForm("pass"));
				stream << form.open();
				HTMLTable t;
				stream << t.open();
				stream << t.row();
				stream << t.col() << "Mot de passe";
				stream << t.col() << form.getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS1, "");
				stream << t.row();
				stream << t.col() << "Mot de passe (vérification)";
				stream << t.col() << form.getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS2, "");
				stream << t.row();
				stream << t.col(2) << form.getSubmitButton("Changer le mot de passe");
				stream << t.close();
				stream << form.close();
			}
		}

		void UserAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY));
				if (id != UNKNOWN_VALUE && id != QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
					_user = UserTableSync::Get(id,GET_AUTO, true);
			}
			catch (...)
			{
				throw AdminParametersException("Bad user");
			}
		}

		bool UserAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}
	}
}
