
/** DeleteRightAction class implementation.
	@file DeleteRightAction.cpp

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

#include "DeleteRightAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "ProfileTableSync.h"
#include "Request.h"
#include "Right.h"
#include "SecurityModule.h"
#include "SecurityRight.h"
#include "Session.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, security::DeleteRightAction>::FACTORY_KEY("dra");

	namespace security
	{
		const string DeleteRightAction::PARAMETER_PROFILE_ID(Action_PARAMETER_PREFIX + "p");
		const string DeleteRightAction::PARAMETER_RIGHT = Action_PARAMETER_PREFIX + "right";
		const string DeleteRightAction::PARAMETER_PARAMETER(Action_PARAMETER_PREFIX + "m");

		ParametersMap DeleteRightAction::getParametersMap() const
		{
			ParametersMap map;
			if (_right.get())
			{
				map.insert(PARAMETER_RIGHT, _right->getFactoryKey());
				map.insert(PARAMETER_PARAMETER, _right->getParameter());
			}
			if(_profile.get()) map.insert(PARAMETER_PROFILE_ID, _profile->getKey());
			return map;
		}

		void DeleteRightAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_profile = ProfileTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_PROFILE_ID),
					*_env
				);
			}
			catch(...)
			{
				throw ActionException("Profile not found");
			}

			string rightCode(map.get<string>(PARAMETER_RIGHT));
			string parameter(map.get<string>(PARAMETER_PARAMETER));

			_right = _profile->getRight(rightCode, parameter);
			if (!_right.get())
				throw ActionException("Specified right not found");
		}

		void DeleteRightAction::run(Request& request)
		{
			if (_profile != NULL)
			{
				_profile->removeRight(_right->getFactoryKey(), _right->getParameter());
				ProfileTableSync::Save(_profile.get());
			}
		}



		bool DeleteRightAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE);
		}


		void DeleteRightAction::setProfile(boost::shared_ptr<const Profile> value)
		{
			_profile = const_pointer_cast<Profile>(value);
		}
	}
}
