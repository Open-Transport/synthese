////////////////////////////////////////////////////////////////////////////////
/// DeleteProfileAction class implementation.
///	@file DeleteProfileAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "DeleteProfileAction.h"
#include "UserTableSync.h"
#include "ProfileTableSync.h"
#include "SecurityLog.h"
#include "SecurityRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;	
	
	template<> const std::string util::FactorableTemplate<Action,security::DeleteProfileAction>::FACTORY_KEY("dpa");

	namespace security
	{
		const string DeleteProfileAction::PARAMETER_PROFILE_ID(Action_PARAMETER_PREFIX + "p");
		
		ParametersMap DeleteProfileAction::getParametersMap() const
		{
			ParametersMap m;
			if(_profile.get()) m.insert(PARAMETER_PROFILE_ID, _profile->getKey());
			return m;
		}

		void DeleteProfileAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_profile = ProfileTableSync::Get(
					map.get<RegistryKeyType>(PARAMETER_PROFILE_ID),
					*_env
				);
			}
			catch(...)
			{
				throw ActionException("Specified Profile not found");
			}

			// Search of child profiles
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(*_env, _profile.get() ? _profile->getKey() : 0, 0, 1, FIELDS_ONLY_LOAD_LEVEL)
			);
			if(!profiles.empty())
			{
				throw ActionException("Au moins un profil hérite du profil spécifié. La suppression est impossible.");
			}

			// Search of users
			UserTableSync::Search(
				*_env,
				optional<string>(),
				optional<string>(),
				optional<string>(),
				optional<string>(),
				_profile->getKey(),
				boost::logic::indeterminate,
				boost::logic::indeterminate,
				optional<RegistryKeyType>(),
				0, 1,
				false, false, false, false,
				FIELDS_ONLY_LOAD_LEVEL
			);
			if (!_env->getRegistry<User>().empty())
				throw ActionException("Au moins un utilisateur appartient au profil spécifié. La suppression est impossible.");
		}

		void DeleteProfileAction::run(Request& request)
		{
			ProfileTableSync::Remove(_profile->getKey());

			// Log
			SecurityLog::addProfileAdmin(request.getUser().get(), _profile.get(), "Suppression de " + _profile->getName());
		}



		bool DeleteProfileAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(DELETE_RIGHT);
		}
		
		void DeleteProfileAction::setProfile(boost::shared_ptr<Profile> value)
		{
			_profile = const_pointer_cast<const Profile>(value);
		}
	}
}
