
/** AddUserAction class implementation.
	@file AddUserAction.cpp

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

#include "01_util/Conversion.h"

#include "12_security/SecurityModule.h"
#include "12_security/Profile.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/AddUserAction.h"

#include "30_server/ActionException.h"

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace security
	{
		const std::string AddUserAction::PARAMETER_NAME = "actionParamnm";
		const std::string AddUserAction::PARAMETER_LOGIN = "actionParamlg";
		const std::string AddUserAction::PARAMETER_PROFILE_ID = "actionParampid";

		Request::ParametersMap AddUserAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_LOGIN, _login));
			map.insert(make_pair(PARAMETER_PROFILE_ID, Conversion::ToString(_profile->getKey())));
			return map;
		}

		void AddUserAction::setFromParametersMap(Request::ParametersMap& map )
		{
			try
			{
				Request::ParametersMap::iterator it;
				
				it = map.find(PARAMETER_NAME);
				if (it == map.end())
					throw ActionException("Name not specified");
				_name = it->second;
				if (_name.empty())
					throw ActionException("Le nom de l'utilisateur ne peut être vide");
				map.erase(it);

				it = map.find(PARAMETER_LOGIN);
				if (it == map.end())
					throw ActionException("Login not specified");
                _login = it->second;
				if (_login.empty())
					throw ActionException("Le login ne peut être vide");
				if (UserTableSync::loginExists(_login))
					throw ActionException("Le login demandé est déjà utilisé");
				map.erase(it);

				it = map.find(PARAMETER_PROFILE_ID);
				if (it == map.end())
					throw ActionException("Profile not specified");
				_profile = SecurityModule::getProfiles().get(Conversion::ToLongLong(it->second));
				map.erase(it);
			}
			catch (Profile::RegistryKeyException e)
			{
				throw ActionException("Profil inexistant");
			}

		}

		void AddUserAction::run()
		{
			User* user = new User;
			user->setLogin(_login);
			user->setName(_name);
			user->setProfile(_profile);
			UserTableSync::save(user);
			_request->setObjectId(user->getKey());
			delete user;
		}
	}
}
