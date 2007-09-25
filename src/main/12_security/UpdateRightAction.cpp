
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
#include "12_security/Right.h"
#include "12_security/SecurityLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "13_dblog/DBLogModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action,security::UpdateRightAction>::FACTORY_KEY("ura");
	}
	
	namespace security
	{
		const string UpdateRightAction::PARAMETER_RIGHT_CODE = Action_PARAMETER_PREFIX + "co";
		const string UpdateRightAction::PARAMETER_RIGHT_PARAMETER = Action_PARAMETER_PREFIX + "pr";
		const string UpdateRightAction::PARAMETER_PUBLIC_VALUE = Action_PARAMETER_PREFIX + "uv";
		const string UpdateRightAction::PARAMETER_PRIVATE_VALUE = Action_PARAMETER_PREFIX + "rv";
		
		ParametersMap UpdateRightAction::getParametersMap() const
		{
			ParametersMap map;
			if (_right.get())
			{
				map.insert(make_pair(PARAMETER_RIGHT_CODE, _right->getFactoryKey()));
				map.insert(make_pair(PARAMETER_RIGHT_PARAMETER, _right->getParameter()));
				map.insert(make_pair(PARAMETER_PUBLIC_VALUE, Conversion::ToString(static_cast<int>(_right->getPublicRightLevel()))));
				map.insert(make_pair(PARAMETER_PRIVATE_VALUE, Conversion::ToString(static_cast<int>(_right->getPrivateRightLevel()))));
			}
			return map;
		}

		void UpdateRightAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			try
			{
				_profile = ProfileTableSync::get(_request->getObjectId());
			}
			catch(...)
			{
				throw ActionException("Profile not found");
			}

			// Right code
			string rightCode(Request::getStringFormParameterMap(map, PARAMETER_RIGHT_CODE, true, FACTORY_KEY));

			// Right parameter
			string parameter(Request::getStringFormParameterMap(map, PARAMETER_RIGHT_PARAMETER, true, FACTORY_KEY));
			_right = _profile->getRight(rightCode, parameter);
			if (!_right.get())
				throw ActionException("Specified right not found on profile");

			// Public level
			_publicLevel = static_cast<RightLevel>(Request::getIntFromParameterMap(map, PARAMETER_PUBLIC_VALUE, true, FACTORY_KEY));

			// Private level
			_privateLevel = static_cast<RightLevel>(Request::getIntFromParameterMap(map, PARAMETER_PRIVATE_VALUE, false, FACTORY_KEY));
		}

		void UpdateRightAction::run()
		{
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Droits publics", Right::getLevelLabel(_right->getPublicRightLevel()), Right::getLevelLabel(_publicLevel));
			DBLogModule::appendToLogIfChange(log, "Droits privés", Right::getLevelLabel(_right->getPrivateRightLevel()), Right::getLevelLabel(_privateLevel));

			_right->setPrivateLevel(_privateLevel);
			_right->setPublicLevel(_publicLevel);

			ProfileTableSync::save(_profile.get());

			SecurityLog::addProfileAdmin(_request->getUser(), _profile, _right->getFactoryKey() + "/" + _right->getParameter() + log.str());
		}
	}
}
