
/** UserUpdateAction class implementation.
	@file UserUpdateAction.cpp

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

#include "UserUpdateAction.h"

#include "12_security/UserTableSync.h"
#include "12_security/ProfileTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

#include "02_db/DBEmptyResultException.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;

	template<> const string util::FactorableTemplate<Action, security::UserUpdateAction>::FACTORY_KEY("uua");
	
	namespace security
	{
		const std::string UserUpdateAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const std::string UserUpdateAction::PARAMETER_SURNAME = Action_PARAMETER_PREFIX + "surn";
		const std::string UserUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const std::string UserUpdateAction::PARAMETER_ADDRESS = Action_PARAMETER_PREFIX + "addr";
		const std::string UserUpdateAction::PARAMETER_POSTAL_CODE = Action_PARAMETER_PREFIX + "post";
		const std::string UserUpdateAction::PARAMETER_CITY = Action_PARAMETER_PREFIX + "city";
		const std::string UserUpdateAction::PARAMETER_PHONE = Action_PARAMETER_PREFIX + "phon";
		const std::string UserUpdateAction::PARAMETER_EMAIL = Action_PARAMETER_PREFIX + "email";
		const std::string UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN = Action_PARAMETER_PREFIX + "auth";
		const std::string UserUpdateAction::PARAMETER_PROFILE_ID = Action_PARAMETER_PREFIX + "prof";


		ParametersMap UserUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UserUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::GetUpdateable(_request->getObjectId());

				_login = map.getString(PARAMETER_LOGIN, true, FACTORY_KEY);
				if (_login.empty())
					throw ActionException("Le login ne peut être vide");
				// Put a control of unicity

				_surname = map.getString(PARAMETER_SURNAME, true, FACTORY_KEY);

				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
				if (_name.empty())
					throw ActionException("Le nom de l'utilisateur ne peut être vide");

				_address = map.getString(PARAMETER_ADDRESS, true, FACTORY_KEY);

				_postalCode = map.getString(PARAMETER_POSTAL_CODE, true, FACTORY_KEY);

				_phone = map.getString(PARAMETER_PHONE, true, FACTORY_KEY);

				_city = map.getString(PARAMETER_CITY, true, FACTORY_KEY);

				_email = map.getString(PARAMETER_EMAIL, true, FACTORY_KEY);

				_authorizedLogin = map.getBool(PARAMETER_AUTHORIZED_LOGIN, true, false, FACTORY_KEY);

				uid id(map.getUid(PARAMETER_PROFILE_ID, true, FACTORY_KEY));
				_profile = ProfileTableSync::Get(id);
			}
			catch (Profile::RegistryKeyException)
			{
				throw ActionException("Profil introuvable");
			}
			catch (DBEmptyResultException<User>)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}

		void UserUpdateAction::run()
		{
			_user->setLogin(_login);
			_user->setAddress(_address);
			_user->setEMail(_email);
			_user->setPostCode(_postalCode);
			_user->setCityText(_city);
			_user->setPhone(_phone);
			_user->setProfile(_profile.get());
			_user->setConnectionAllowed(_authorizedLogin);
			_user->setName(_name);
			_user->setSurname(_surname);
			UserTableSync::save(_user.get());
		}
	}
}
