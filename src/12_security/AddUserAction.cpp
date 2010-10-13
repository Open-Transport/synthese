
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
#include "ProfileTableSync.h"
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
		}

		void AddUserAction::run(Request& request)
		{
			User user;
			user.setLogin(_login);
			user.setName(_name);
			user.setProfile(_profile.get());
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
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE);
			/// @todo Add a control on the profile on the user who creates the new user, depending on the new user profile
		}
	}
}
