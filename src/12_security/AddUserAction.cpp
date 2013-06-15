
/** AddUserAction class implementation.
	@file AddUserAction.cpp

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

#include "AddUserAction.h"

#include "ProfileTableSync.h"
#include "Session.h"
#include "UserTableSync.h"
#include "Profile.h"
#include "SecurityRight.h"
#include "SecurityLog.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<> const std::string util::FactorableTemplate<Action, security::AddUserAction>::FACTORY_KEY("sau");

	namespace security
	{
		const std::string AddUserAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "nm");
		const std::string AddUserAction::PARAMETER_LOGIN(Action_PARAMETER_PREFIX + "lg");
		const std::string AddUserAction::PARAMETER_PROFILE_ID(Action_PARAMETER_PREFIX + "pid");
		const std::string AddUserAction::PARAMETER_PASS1(Action_PARAMETER_PREFIX + "p1");
		const std::string AddUserAction::PARAMETER_PASS2(Action_PARAMETER_PREFIX + "p2");
		const std::string AddUserAction::PARAMETER_SURNAME(Action_PARAMETER_PREFIX + "surn");
		const std::string AddUserAction::PARAMETER_ADDRESS(Action_PARAMETER_PREFIX + "addr");
		const std::string AddUserAction::PARAMETER_POSTAL_CODE(Action_PARAMETER_PREFIX + "post");
		const std::string AddUserAction::PARAMETER_CITY(Action_PARAMETER_PREFIX + "city");
		const std::string AddUserAction::PARAMETER_COUNTRY(Action_PARAMETER_PREFIX + "country");
		const std::string AddUserAction::PARAMETER_PHONE(Action_PARAMETER_PREFIX + "phon");
		const std::string AddUserAction::PARAMETER_EMAIL(Action_PARAMETER_PREFIX + "email");

		ParametersMap AddUserAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_LOGIN, _login);
			if (_profile.get())
				map.insert(PARAMETER_PROFILE_ID, _profile->getKey());
			return map;
		}

		void AddUserAction::_setFromParametersMap(const ParametersMap& map )
		{
			_name = map.get<string>(PARAMETER_NAME);
			if (_name.empty())
				throw ActionException("L'utilisateur ne peut être créé car le nom n'est pas renseigné. Veuillez renseigner le champ nom.");

			_login = map.get<string>(PARAMETER_LOGIN);
			if (_login.empty())
				throw ActionException("L'utilisateur ne peut être créé car le login n'est pas renseigné. Veuillez renseigner le champ login.");
			if (UserTableSync::loginExists(_login))
				throw ActionException("L'utilisateur ne peut être créé car le login entré est déjà utilisé. Veuillez choisir un autre login.");

			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_PROFILE_ID));
			try
			{
				_profile = ProfileTableSync::Get(id,*_env);
			}
			catch (...)
			{
				throw ActionException("Profil inexistant");
			}

			_password = map.getDefault<string>(PARAMETER_PASS1, string(), false);
			if (!_password.empty())
			{
				string pass2 = map.getDefault<string>(PARAMETER_PASS2, string(), false);
				if (pass2 != _password)
					throw ActionException("Les mots de passe entrés ne sont pas identiques");
			}
			
			_surname = map.getDefault<string>(PARAMETER_SURNAME);
			_address = map.getDefault<string>(PARAMETER_ADDRESS);
			_postalCode = map.getDefault<string>(PARAMETER_POSTAL_CODE);
			_phone = map.getDefault<string>(PARAMETER_PHONE);
			_city = map.getDefault<string>(PARAMETER_CITY);
			_country = map.getDefault<string>(PARAMETER_COUNTRY);
			_email = map.getDefault<string>(PARAMETER_EMAIL);
		}

		void AddUserAction::run(Request& request)
		{
			User user;
			user.setLogin(_login);
			user.setName(_name);
			user.setProfile(_profile.get());
			if (!_password.empty())
				user.setPassword(_password);
			if (!_surname.empty())
				user.setSurname(_surname);
			if (!_address.empty())
				user.setAddress(_address);
			if (!_postalCode.empty())
				user.setPostCode(_postalCode);
			if (!_phone.empty())
				user.setPhone(_phone);
			if (!_city.empty())
				user.setCityText(_city);
			if (!_country.empty())
				user.setCountry(_country);
			if (!_email.empty())
				user.setEMail(_email);
			UserTableSync::Save(&user);

			request.setActionCreatedId(user.getKey());

			// DBLog
			SecurityLog::addUserAdmin(
				request.getUser().get(),
				&user,
				"Création de l'utilisateur " + user.getLogin()
			);
		}



		bool AddUserAction::isAuthorized(const Session* session
		) const {
			// Special patch for tmr_groupes which allow user to subscribe directly
			if (_profile.get()->getName() == "Utilisateurs TMR")
				return true;
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE);
			/// @todo Add a check on the profile on the user who creates the new user, depending on the new user profile
		}
	}
}
