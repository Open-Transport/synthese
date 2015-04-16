
/** UpdateProfileAction class implementation.
	@file UpdateProfileAction.cpp

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

#include "UpdateProfileAction.h"

#include "SecurityModule.h"
#include "Session.h"
#include "User.h"
#include "ProfileTableSync.h"
#include "SecurityLog.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DBLogModule.h"
#include "SecurityRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace dblog;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action,security::UpdateProfileAction>::FACTORY_KEY("upa");
	}

	namespace security
	{
		const string UpdateProfileAction::PARAMETER_PROFILE_ID = Action_PARAMETER_PREFIX + "p";
		const string UpdateProfileAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";


		ParametersMap UpdateProfileAction::getParametersMap() const
		{
			ParametersMap map;
			if(_profile.get()) map.insert(PARAMETER_PROFILE_ID, _profile->getKey());
			map.insert(PARAMETER_NAME, _name);
			return map;
		}

		void UpdateProfileAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Profile
			try
			{
				_profile = ProfileTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_PROFILE_ID),
					*_env
				);
			}
			catch (ObjectNotFoundException<Profile>& e)
			{
				throw ActionException(e.getMessage());
			}

			// Name
			_name = map.get<string>(PARAMETER_NAME);
			Env env;
			ProfileTableSync::Search(env, _name, string(), 0,1);
			if (!env.getRegistry<Profile>().empty())
				throw ActionException("Le nom choisi est déjà pris par un autre profil. Veuillez entrer un autre nom.");
		}

		void UpdateProfileAction::run(Request& request)
		{
			// Old value
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _profile->getName(), _name);

			// Action
			_profile->setName(_name);
			ProfileTableSync::Save(_profile.get());

			// Log
			SecurityLog::addProfileAdmin(request.getUser().get(), _profile.get(), log.str());
		}



		bool UpdateProfileAction::isAuthorized(const Session* session
		) const {
			/// @todo add a check on the users profile
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE);
		}


		void UpdateProfileAction::setProfile(boost::shared_ptr<const Profile> value)
		{
			_profile = const_pointer_cast<Profile>(value);
		}
	}
}
