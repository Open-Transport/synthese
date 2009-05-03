
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
#include "RequestMissingParameterException.h"
#include "UserAdmin.h"
#include "UsersAdmin.h"
#include "User.h"
#include "UserTableSync.h"
#include "SecurityModule.h"
#include "UserUpdateAction.h"
#include "UserPasswordUpdateAction.h"
#include "ActionFunctionRequest.h"
#include "Request.h"
#include "AdminParametersException.h"
#include "AdminRequest.h"
#include "Conversion.h"

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
			bool doDisplayPreparationActions
		){
			try
			{
				uid id(map.getUid(Request::PARAMETER_OBJECT_ID, false, FACTORY_KEY));
				if (id != UNKNOWN_VALUE && id != Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				{
					_user = UserTableSync::Get(id, _env, UP_LINKS_LOAD_LEVEL);
				}
			}
			catch (ObjectNotFoundException<User>& e)
			{
				throw AdminParametersException(e.getMessage());
			}
		}



		server::ParametersMap UserAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(Request::PARAMETER_OBJECT_ID, _request->getObjectId());
			return m;
		}



		void UserAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables) const
		{
			ActionFunctionRequest<UserUpdateAction, AdminRequest> updateRequest(_request);
			updateRequest.getFunction()->setPage<UserAdmin>();
			updateRequest.setObjectId(_request->getObjectId());

			ActionFunctionRequest<UserPasswordUpdateAction, AdminRequest> userPasswordUpdateRequest(_request);
			userPasswordUpdateRequest.getFunction()->setPage<UserAdmin>();
			userPasswordUpdateRequest.setObjectId(_request->getObjectId());

			{
				stream << "<h1>Propriétés</h1>";
				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.title("Connexion");
				stream << t.cell("Login", t.getForm().getTextInput(UserUpdateAction::PARAMETER_LOGIN, _user->getLogin()));
				
				stream << t.title("Coordonnées");
				stream << t.cell("Prénom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_SURNAME, _user->getSurname()));
				stream << t.cell("Nom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_NAME, _user->getName()));
				stream << t.cell("Adresse", t.getForm().getTextAreaInput(UserUpdateAction::PARAMETER_ADDRESS, _user->getAddress(), 4, 50));
				stream << t.cell("Code postal", t.getForm().getTextInput(UserUpdateAction::PARAMETER_POSTAL_CODE, _user->getPostCode()));
				stream << t.cell("Ville", t.getForm().getTextInput(UserUpdateAction::PARAMETER_CITY, _user->getCityText()));
				stream << t.cell("Téléphone",t.getForm().getTextInput(UserUpdateAction::PARAMETER_PHONE, _user->getPhone()));
				stream << t.cell("E-mail",t.getForm().getTextInput(UserUpdateAction::PARAMETER_EMAIL, _user->getEMail()));
				
				stream << t.title("Droits");
				stream << t.cell("Connexion autorisée",t.getForm().getOuiNonRadioInput(UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN, _user->getConnectionAllowed()));
				stream << t.cell("Profil",t.getForm().getSelectInput(UserUpdateAction::PARAMETER_PROFILE_ID, SecurityModule::getProfileLabels(), _user->getProfile()->getKey()));
				stream << t.close();
			}

			stream << "<h1>Changement de mot de passe</h1>";
			{
				PropertiesHTMLTable t(userPasswordUpdateRequest.getHTMLForm("pass"));
				stream << t.open();
				stream << t.cell("Mot de passe", t.getForm().getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS1, ""));
				stream << t.cell("Mot de passe (vérification)", t.getForm().getPasswordInput(UserPasswordUpdateAction::PARAMETER_PASS2, ""));
				stream << t.close();
			}
		}



		bool UserAdmin::isAuthorized() const
		{
			return true;
		}

		AdminInterfaceElement::PageLinks UserAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == UsersAdmin::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY)
			{
				links.push_back(currentPage.getPageLink());
			}
			return links;
		}

		std::string UserAdmin::getTitle() const
		{
			return _user.get() ? _user->getSurname() + " " + _user->getName() : DEFAULT_TITLE;
		}

		std::string UserAdmin::getParameterName() const
		{
			return _user.get() ? Request::PARAMETER_OBJECT_ID : string();
		}

		std::string UserAdmin::getParameterValue() const
		{
			return _user.get() ? Conversion::ToString(_user->getKey()) : string();
		}
	}
}
