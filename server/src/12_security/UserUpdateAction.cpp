////////////////////////////////////////////////////////////////////////////////
/// UserUpdateAction class implementation.
///	@file UserUpdateAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "UserUpdateAction.h"

#include "Profile.h"
#include "SecurityRight.h"
#include "Session.h"
#include "UserTableSync.h"
#include "ProfileTableSync.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "SecurityLog.h"
#include "DBLogModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, security::UserUpdateAction>::FACTORY_KEY("uua");

	namespace security
	{
		const string UserUpdateAction::PARAMETER_USER_ID(Action_PARAMETER_PREFIX + "u");
		const string UserUpdateAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const string UserUpdateAction::PARAMETER_SURNAME = Action_PARAMETER_PREFIX + "surn";
		const string UserUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const string UserUpdateAction::PARAMETER_ADDRESS = Action_PARAMETER_PREFIX + "addr";
		const string UserUpdateAction::PARAMETER_POSTAL_CODE(Action_PARAMETER_PREFIX + "post");
		const string UserUpdateAction::PARAMETER_CITY = Action_PARAMETER_PREFIX + "city";
		const string UserUpdateAction::PARAMETER_PHONE = Action_PARAMETER_PREFIX + "phon";
		const string UserUpdateAction::PARAMETER_EMAIL = Action_PARAMETER_PREFIX + "email";
		const string UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN(Action_PARAMETER_PREFIX + "a");
		const string UserUpdateAction::PARAMETER_PROFILE_ID = Action_PARAMETER_PREFIX + "prof";


		ParametersMap UserUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_user.get()) map.insert(PARAMETER_USER_ID, _user->getKey());
			return map;
		}

		void UserUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_USER_ID),
					*_env
				);

				_login = map.getDefault<string>(PARAMETER_LOGIN);

				if(!_login.empty())
				{
					UserTableSync::SearchResult users(
						UserTableSync::Search(
						*_env,
						_login,
						optional<string>(),
						optional<string>(),
						optional<string>(),
						optional<RegistryKeyType>(),
						logic::indeterminate,
						logic::indeterminate,
						_user->getKey(),
						0, 1, false, false, false, false, FIELDS_ONLY_LOAD_LEVEL
						)	);
					if(!users.empty())
					{
						throw ActionException("Ce login est déjà utilisé.");
					}
				}

				_surname = map.getDefault<string>(PARAMETER_SURNAME);

				_name = map.getDefault<string>(PARAMETER_NAME);
				if (_name.empty())
					throw ActionException("Le nom de l'utilisateur ne peut être vide");

				_address = map.getDefault<string>(PARAMETER_ADDRESS);

				_postalCode = map.getDefault<string>(PARAMETER_POSTAL_CODE);

				_phone = map.getDefault<string>(PARAMETER_PHONE);

				_city = map.getDefault<string>(PARAMETER_CITY);

				_email = map.getDefault<string>(PARAMETER_EMAIL);

				_authorizedLogin = map.getOptional<bool>(PARAMETER_AUTHORIZED_LOGIN);

				if(map.getOptional<RegistryKeyType>(PARAMETER_PROFILE_ID))
				{
					_profile = ProfileTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PROFILE_ID), *_env);
				}

			}
			catch (ObjectNotFoundException<Profile>&)
			{
				throw ActionException("Profil introuvable");
			}
			catch (ObjectNotFoundException<User>&)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}

		void UserUpdateAction::run(Request& request)
		{
			stringstream s;
			DBLogModule::appendToLogIfChange(s, "Login", _user->getLogin(), _login);
			DBLogModule::appendToLogIfChange(s, "Adresse", _user->getAddress(), _address);
			DBLogModule::appendToLogIfChange(s, "E-mail", _user->getEMail(), _email);
			DBLogModule::appendToLogIfChange(s, "Code postal", _user->getPostCode(), _postalCode);
			DBLogModule::appendToLogIfChange(s, "Ville", _user->getCityText(), _city);
			DBLogModule::appendToLogIfChange(s, "Téléphone", _user->getPhone(), _phone);
			if(_profile)
			{
				DBLogModule::appendToLogIfChange(s, "Profil", (_user->getProfile() != NULL) ? _user->getProfile()->getName() : string(), (*_profile != NULL) ? (*_profile)->getName() : string());
			}
			if(_authorizedLogin)
			{
				DBLogModule::appendToLogIfChange(s, "Autorisation de connexion", lexical_cast<string>(_user->getConnectionAllowed()), lexical_cast<string>(*_authorizedLogin));
			}
			DBLogModule::appendToLogIfChange(s, "Nom", _user->getName(), _name);
			DBLogModule::appendToLogIfChange(s, "Prénom", _user->getSurname(), _surname);

			_user->setLogin(_login);
			_user->setAddress(_address);
			_user->setEMail(_email);
			_user->setPostCode(_postalCode);
			_user->setCityText(_city);
			_user->setPhone(_phone);
			if(_profile)
			{
				_user->setProfile(_profile->get());
			}
			if(_authorizedLogin)
			{
				_user->setConnectionAllowed(*_authorizedLogin);
			}
			_user->setName(_name);
			_user->setSurname(_surname);

			UserTableSync::Save(_user.get());

			SecurityLog::addUserAdmin(request.getUser().get(), _user.get(), s.str());
		}



		bool UserUpdateAction::isAuthorized(const Session* session
		) const {
			return
				session &&
				(	(session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE)) ||
					(session->getUser() && session->getUser()->getKey() == _user->getKey())
				)
			;
		}


		void UserUpdateAction::setUser(boost::shared_ptr<const User> value)
		{
			_user = const_pointer_cast<User>(value);
		}
	}
}
