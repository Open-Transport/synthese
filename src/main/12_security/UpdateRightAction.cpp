
/** UpdateRightAction class implementation.
	@file UpdateRightAction.cpp

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

#include "12_security/UpdateRightAction.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/SecurityModule.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace security
	{
		const string UpdateRightAction::PARAMETER_RIGHT_CODE = Action_PARAMETER_PREFIX + "co";
		const string UpdateRightAction::PARAMETER_RIGHT_PARAMETER = Action_PARAMETER_PREFIX + "pr";
		const string UpdateRightAction::PARAMETER_PUBLIC_VALUE = Action_PARAMETER_PREFIX + "uv";
		const string UpdateRightAction::PARAMETER_PRIVATE_VALUE = Action_PARAMETER_PREFIX + "rv";
		
		ParametersMap UpdateRightAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_RIGHT_CODE, _right->getFactoryKey()));
			map.insert(make_pair(PARAMETER_RIGHT_PARAMETER, _right->getParameter()));
			map.insert(make_pair(PARAMETER_PUBLIC_VALUE, Conversion::ToString((int) _right->getPublicRightLevel())));
			map.insert(make_pair(PARAMETER_PRIVATE_VALUE, Conversion::ToString((int) _right->getPrivateRightLevel())));
			return map;
		}

		void UpdateRightAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				// Profile
				_profile = SecurityModule::getProfiles().get(_request->getObjectId());

				// Right code
				string rightCode;
				it = map.find(PARAMETER_RIGHT_CODE);
				if (it == map.end())
					throw ActionException("Right code not specified");
				rightCode = it->second;

				// Right parameter
				it = map.find(PARAMETER_RIGHT_PARAMETER);
				if (it == map.end())
					throw ActionException("Right parameter not specified");

				_right = _profile->getRight(rightCode, it->second);

				// Public level
				it = map.find(PARAMETER_PUBLIC_VALUE);
				if (it == map.end())
					throw ActionException("Public level not specified");
				_publicLevel = (Right::Level) Conversion::ToInt(it->second);

				// Private level
				it = map.find(PARAMETER_PRIVATE_VALUE);
				if (it == map.end())
					throw ActionException("Private level not specified");
				_privateLevel = (Right::Level) Conversion::ToInt(it->second);
			}
			catch (Profile::RegistryKeyException e)
			{
				throw ActionException("Profile not found");
			}
			catch (...)
			{
				throw ActionException("Specified right not found on profile");
			}
		}

		void UpdateRightAction::run()
		{
			_right->setPrivateLevel(_privateLevel);
			_right->setPublicLevel(_publicLevel);
			ProfileTableSync::save(_profile);
		}
	}
}
