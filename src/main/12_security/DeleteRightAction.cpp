
/** DeleteRightAction class implementation.
	@file DeleteRightAction.cpp

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

#include "12_security/DeleteRightAction.h"
#include "12_security/Profile.h"
#include "12_security/Right.h"
#include "12_security/SecurityModule.h"
#include "12_security/ProfileTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace security
	{
		const string DeleteRightAction::PARAMETER_RIGHT = Action_PARAMETER_PREFIX + "right";
		const string DeleteRightAction::PARAMETER_PARAMETER = Action_PARAMETER_PREFIX + "param";

		ParametersMap DeleteRightAction::getParametersMap() const
		{
			ParametersMap map;
			if (_right.get())
			{
				map.insert(make_pair(PARAMETER_RIGHT, _right->getFactoryKey()));
				map.insert(make_pair(PARAMETER_PARAMETER, _right->getParameter()));
			}
			return map;
		}

		void DeleteRightAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			if (!SecurityModule::getProfiles().contains(_request->getObjectId()))
				throw ActionException("Profile not found");
			_profile = SecurityModule::getProfiles().get(_request->getObjectId());

			string rightCode;
			it = map.find(PARAMETER_RIGHT);
			if (it == map.end())
				throw ActionException("Right not specified");
			rightCode = it->second;

			it = map.find(PARAMETER_PARAMETER);
			if (it == map.end())
				throw ActionException("Parameter not specified");
			
			_right.reset(_profile->getRight(rightCode, it->second));
			if (!_right.get())
				throw ActionException("Specified right not found");
		}

		void DeleteRightAction::run()
		{
			if (_profile != NULL)
			{
				_profile->removeRight(_right->getFactoryKey(), _right->getParameter());
				ProfileTableSync::save(_profile);
			}
		}

		DeleteRightAction::DeleteRightAction()
			: Action()
			, _profile(NULL)
		{
	
		}
	}
}
