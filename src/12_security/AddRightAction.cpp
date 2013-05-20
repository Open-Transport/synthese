
/** AddRightAction class implementation.
	@file AddRightAction.cpp

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

#include "AddRightAction.h"

#include "Profile.h"
#include "SecurityModule.h"
#include "Session.h"
#include "User.h"
#include "ProfileTableSync.h"
#include "Right.h"
#include "SecurityLog.h"
#include "SecurityRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, security::AddRightAction>::FACTORY_KEY("ara");
	}

	namespace security
	{
		const string AddRightAction::PARAMETER_PROFILE_ID(Action_PARAMETER_PREFIX + "p");
		const string AddRightAction::PARAMETER_RIGHT = Action_PARAMETER_PREFIX + "r";
		const string AddRightAction::PARAMETER_PUBLIC_LEVEL = Action_PARAMETER_PREFIX + "u";
		const string AddRightAction::PARAMETER_PRIVATE_LEVEL = Action_PARAMETER_PREFIX + "t";
		const string AddRightAction::PARAMETER_PARAMETER = Action_PARAMETER_PREFIX + "f";


		ParametersMap AddRightAction::getParametersMap() const
		{
			ParametersMap map;
			if(_profile.get()) map.insert(PARAMETER_PROFILE_ID, _profile->getKey());
			return map;
		}

		void AddRightAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_profile = ProfileTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_PROFILE_ID),
					*_env
				);

				_rightName = map.get<string>(PARAMETER_RIGHT);
				if (!Factory<Right>::contains(_rightName))
					throw ActionException("Specified right class not found");

				_parameter = map.get<string>(PARAMETER_PARAMETER);
				_publicLevel = static_cast<RightLevel>(
					map.get<int>(PARAMETER_PUBLIC_LEVEL)
				);
				_privateLevel = static_cast<RightLevel>(
					map.getDefault<int>(PARAMETER_PRIVATE_LEVEL, UNKNOWN_VALUE)
				);
			}
			catch(ObjectNotFoundException<Profile>&)
			{
				throw ActionException("Profil introuvable");
			}
		}

		void AddRightAction::run(
			Request& request
		){
			boost::shared_ptr<Right> right(Factory<Right>::create(_rightName));
			right->setParameter(_parameter);
			right->setPrivateLevel(_privateLevel);
			right->setPublicLevel(_publicLevel);
			_profile->addRight(right);

			ProfileTableSync::Save(_profile.get());

			SecurityLog::addProfileAdmin(request.getUser().get(), _profile.get(), "Ajout habilitation " + _rightName + "/" + _parameter);
		}



		bool AddRightAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE);
			/// @todo Add a check on the profile on the user who creates the new profile
		}

		void AddRightAction::setProfile(boost::shared_ptr<Profile> value)
		{
			_profile = value;
		}



		void AddRightAction::setProfile(boost::shared_ptr<const Profile> value)
		{
			_profile = const_pointer_cast<Profile>(value);
		}
}
}
