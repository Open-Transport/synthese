
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

#include "AddUserAction.h"

#include "12_security/ProfileTableSync.h"
#include "12_security/UserTableSync.h"
#include "12_security/Profile.h"
#include "SecurityRight.h"
#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using boost::shared_ptr;

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
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			if (_name.empty())
				throw ActionException("L'utilisateur ne peut être créé car le nom n'est pas renseigné. Veuillez renseigner le champ nom.");
			
			_login = map.getString(PARAMETER_LOGIN, true, FACTORY_KEY);
			if (_login.empty())
				throw ActionException("L'utilisateur ne peut être créé car le login n'est pas renseigné. Veuillez renseigner le champ login.");
			if (UserTableSync::loginExists(_login))
				throw ActionException("L'utilisateur ne peut être créé car le login entré est déjà utilisé. Veuillez choisir un autre login.");
			
			uid id(map.getUid(PARAMETER_PROFILE_ID, true, FACTORY_KEY));
			try
			{
				_profile = ProfileTableSync::Get(id,*_env);
			}
			catch (...)
			{
				throw ActionException("Profil inexistant");
			}
		}

		void AddUserAction::run()
		{
			shared_ptr<User> user(new User);
			user->setLogin(_login);
			user->setName(_name);
			user->setProfile(_profile.get());
			UserTableSync::Save(user.get());
			
						
			if(_request->getObjectId() == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
			{
				_request->setObjectId(user->getKey());
			}
		}



		bool AddUserAction::_isAuthorized(
		) const {
			return _request->isAuthorized<SecurityRight>(WRITE);
			/// @todo Add a control on the profile on the user who creates the new user, depending on the new user profile
		}
	}
}
