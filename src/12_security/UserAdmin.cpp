
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

#include "PropertiesHTMLTable.h"
#include "UserAdmin.h"
#include "UsersAdmin.h"
#include "User.h"
#include "UserTableSync.h"
#include "SecurityModule.h"
#include "UserUpdateAction.h"
#include "UserPasswordUpdateAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminRequest.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "SecurityRight.h"

using namespace std;
using namespace boost;

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
		template<> const string AdminInterfaceElementTemplate<UserAdmin>::DEFAULT_TITLE("Utilisateur inconnu");
	}
	
	namespace security
	{
		UserAdmin::UserAdmin()
			: AdminInterfaceElementTemplate<UserAdmin>()
			, _userError(false)
		{
		}


		
		void UserAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			if(objectWillBeCreatedLater) return;
			
			try
			{
				_user = UserTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (ObjectNotFoundException<User>& e)
			{
				throw AdminParametersException(e.getMessage());
			}
		}



		server::ParametersMap UserAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_user.get()) m.insert(Request::PARAMETER_OBJECT_ID, _user->getKey());
			return m;
		}



		void UserAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request) const
		{
			AdminActionFunctionRequest<UserUpdateAction, UserAdmin> updateRequest(_request);
			updateRequest.getAction()->setUser(_user);
			
			AdminActionFunctionRequest<UserPasswordUpdateAction, UserAdmin> userPasswordUpdateRequest(_request);
			userPasswordUpdateRequest.getAction()->setUserC(_user);

			bool writeRights(
				_request.isAuthorized<SecurityRight>(
					WRITE,
					UNKNOWN_RIGHT_LEVEL,
					_user->getProfile() ? lexical_cast<string>(_user->getProfile()->getKey()) : GLOBAL_PERIMETER
				) ||
				_user->getKey() == _request.getUser()->getKey() &&
				_request.isAuthorized<SecurityRight>(
					UNKNOWN_RIGHT_LEVEL,
					WRITE,
					string()
			)	);


			{
				stream << "<h1>Propri�t�s</h1>";
				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				t.getForm().setUpdateRight(writeRights);
				stream << t.open();
				stream << t.title("Connexion");
				stream << t.cell("Login", t.getForm().getTextInput(UserUpdateAction::PARAMETER_LOGIN, _user->getLogin()));
				
				stream << t.title("Coordonn�es");
				stream << t.cell("Pr�nom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_SURNAME, _user->getSurname()));
				stream << t.cell("Nom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_NAME, _user->getName()));
				stream << t.cell("Adresse", t.getForm().getTextAreaInput(UserUpdateAction::PARAMETER_ADDRESS, _user->getAddress(), 4, 50));
				stream << t.cell("Code postal", t.getForm().getTextInput(UserUpdateAction::PARAMETER_POSTAL_CODE, _user->getPostCode()));
				stream << t.cell("Ville", t.getForm().getTextInput(UserUpdateAction::PARAMETER_CITY, _user->getCityText()));
				stream << t.cell("T�l�phone",t.getForm().getTextInput(UserUpdateAction::PARAMETER_PHONE, _user->getPhone()));
				stream << t.cell("E-mail",t.getForm().getTextInput(UserUpdateAction::PARAMETER_EMAIL, _user->getEMail()));
				
				if(	_request.isAuthorized<SecurityRight>(
						WRITE,
						UNKNOWN_RIGHT_LEVEL,
						_user->getProfile() ? lexical_cast<string>(_user->getProfile()->getKey()) : GLOBAL_PERIMETER
				)	){
					stream << t.title("Droits");
					stream << t.cell("Connexion autoris�e",t.getForm().getOuiNonRadioInput(UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN, _user->getConnectionAllowed()));
					stream << t.cell("Profil",t.getForm().getSelectInput(UserUpdateAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), _user->getProfile()->getKey()));
				}
				stream << t.close();
			}

			if(writeRights)
			{
				stream << "<h1>Changement de mot de passe</h1>";
	
				PropertiesHTMLTable t(userPasswordUpdateRequest.getHTMLForm("pass"));
				stream << t.open();
				stream << t.cell("Mot de passe", t.getForm().getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS1, ""));
				stream << t.cell("Mot de passe (v�rification)", t.getForm().getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS2, ""));
				stream << t.close();
			}
		}



		bool UserAdmin::isAuthorized(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{
			if(_request.getActionWillCreateObject()) return true;
			return 	_request.isAuthorized<SecurityRight>(
						READ,
						UNKNOWN_RIGHT_LEVEL,
						_user->getProfile() ? lexical_cast<string>(_user->getProfile()->getKey()) : GLOBAL_PERIMETER
					) ||
					_user->getKey() == _request.getUser()->getKey() &&
					_request.isAuthorized<SecurityRight>(
						UNKNOWN_RIGHT_LEVEL,
						READ,
						string()
				);
		}



		std::string UserAdmin::getTitle() const
		{
			return _user.get() ? _user->getSurname() + " " + _user->getName() : DEFAULT_TITLE;
		}
		
		
		void UserAdmin::setUser(shared_ptr<User> value)
		{
			_user = value;
		}

		void UserAdmin::setUserC(shared_ptr<const User> value)
		{
			_user = const_pointer_cast<const User>(value);
		}


		boost::shared_ptr<const User> UserAdmin::getUser() const
		{
			return _user;
		}
		
		
		bool UserAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _user == static_cast<const UserAdmin&>(other)._user;
		}

	}
}
