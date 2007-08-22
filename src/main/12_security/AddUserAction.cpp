
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

#include "12_security/Profile.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/AddUserAction.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace security
	{
		const std::string AddUserAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "nm");
		const std::string AddUserAction::PARAMETER_LOGIN(Action_PARAMETER_PREFIX + "lg");
		const std::string AddUserAction::PARAMETER_PROFILE_ID(Action_PARAMETER_PREFIX + "pid");

		ParametersMap AddUserAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_LOGIN, _login));
			if (_profile.get())
				map.insert(make_pair(PARAMETER_PROFILE_ID, Conversion::ToString(_profile->getKey())));
			return map;
		}

		void AddUserAction::_setFromParametersMap(const ParametersMap& map )
		{
			ParametersMap::const_iterator it;
			
			it = map.find(PARAMETER_NAME);
			if (it == map.end())
				throw ActionException("Name not specified");
			_name = it->second;
			if (_name.empty())
				throw ActionException("L'utilisateur ne peut être créé car le nom n'est pas renseigné. Veuillez renseigner le champ nom.");
			
			it = map.find(PARAMETER_LOGIN);
			if (it == map.end())
				throw ActionException("Login not specified");
            _login = it->second;
			if (_login.empty())
				throw ActionException("L'utilisateur ne peut être créé car le login n'est pas renseigné. Veuillez renseigner le champ login.");
			if (UserTableSync::loginExists(_login))
				throw ActionException("L'utilisateur ne peut être créé car le login entré est déjà utilisé. Veuillez choisir un autre login.");
			
			it = map.find(PARAMETER_PROFILE_ID);
			if (it == map.end())
				throw ActionException("Profile not specified");
			if (!Profile::Contains(Conversion::ToLongLong(it->second)))
				throw ActionException("Profil inexistant");
			_profile = Profile::Get(Conversion::ToLongLong(it->second));

			_request->setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void AddUserAction::run()
		{
			shared_ptr<User> user(new User);
			user->setLogin(_login);
			user->setName(_name);
			user->setProfile(_profile);
			UserTableSync::save(user.get());
			_request->setObjectId(user->getKey());
		}
	}
}
