
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

#include "30_server/ActionException.h"

#include "12_security/UserUpdateAction.h"
#include "12_security/UserTableSync.h"
#include "12_security/Profile.h"
#include "12_security/User.h"
#include "12_security/SecurityModule.h"

#include "02_db/DBEmptyResultException.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	
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


		Request::ParametersMap UserUpdateAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UserUpdateAction::setFromParametersMap(Request::ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::get(_request->getObjectId());

				Request::ParametersMap::iterator it;

				it = map.find(PARAMETER_LOGIN);
				if (it == map.end())
					throw ActionException("Login not specified");
				_login = it->second;
				if (_login.empty())
					throw ActionException("Le login ne peut être vide");
				// Put a control of unicity
				map.erase(it);

				it = map.find(PARAMETER_SURNAME);
				if (it == map.end())
					throw ActionException("Prénom non spécifié");
				_surname = it->second;
				map.erase(it);

				it = map.find(PARAMETER_NAME);
				if (it == map.end())
					throw ActionException("Nom non spécifié");
				_name = it->second;
				if (_name == "")
					throw ActionException("Le nom de l'utilisateur ne peut être vide");
				map.erase(it);

				it = map.find(PARAMETER_ADDRESS);
				if (it == map.end())
					throw ActionException("Adress not specified");
				_address = it->second;
				map.erase(it);

				it = map.find(PARAMETER_POSTAL_CODE);
				if (it == map.end())
					throw ActionException("Post code not specified");
				_postalCode = it->second;
				map.erase(it);

				it = map.find(PARAMETER_PHONE);
				if (it == map.end())
					throw ActionException("Phone not specified");
				_phone = it->second;
				map.erase(it);

				it = map.find(PARAMETER_CITY);
				if (it == map.end())
					throw ActionException("City not specified");
				_city = it->second;
				map.erase(it);

				it = map.find(PARAMETER_EMAIL);
				if (it == map.end())
					throw ActionException("E-Mail not specified");
				_email = it->second;
				map.erase(it);

				it = map.find(PARAMETER_AUTHORIZED_LOGIN);
				if (it == map.end())
					throw ActionException("Authorized login not specified");
				_authorizedLogin = Conversion::ToBool(it->second);
				map.erase(it);

				it = map.find(PARAMETER_PROFILE_ID);
				if (it == map.end())
					throw ActionException("Profile not specified");
				_profile = SecurityModule::getProfiles().get(Conversion::ToLongLong(it->second));
				map.erase(it);
			}
			catch (Profile::RegistryKeyException e)
			{
				throw ActionException("Profil introuvable");
			}
			catch (DBEmptyResultException e)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}

		UserUpdateAction::UserUpdateAction()
			: Action()
			, _profile(NULL)
		{}

		void UserUpdateAction::run()
		{
			_user->setLogin(_login);
			_user->setAddress(_address);
			_user->setEMail(_email);
			_user->setPostCode(_postalCode);
			_user->setCityText(_city);
			_user->setPhone(_phone);
			_user->setProfile(_profile);
			_user->setConnectionAllowed(_authorizedLogin);
			_user->setName(_name);
			_user->setSurname(_surname);
			UserTableSync::save(_user);
		}

		UserUpdateAction::~UserUpdateAction()
		{
			delete _user;
			delete _profile;
		}
	}
}
