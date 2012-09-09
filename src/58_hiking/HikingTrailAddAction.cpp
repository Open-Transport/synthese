
//////////////////////////////////////////////////////////////////////////
/// HikingTrailAddAction class implementation.
/// @file HikingTrailAddAction.cpp
/// @author Hugues Romain
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "HikingTrailAddAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "HikingRight.h"
#include "Request.h"
#include "HikingTrailTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, hiking::HikingTrailAddAction>::FACTORY_KEY("HikingTrailAddAction");
	}

	namespace hiking
	{
		const string HikingTrailAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";



		ParametersMap HikingTrailAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			return map;
		}



		void HikingTrailAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.getDefault<string>(PARAMETER_NAME);
		}



		void HikingTrailAddAction::run(
			Request& request
		){
			HikingTrail object;
			object.setName(_name);
			HikingTrailTableSync::Save(&object);
			//::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(object.getKey());
		}



		bool HikingTrailAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<HikingRight>(WRITE);
		}
	}
}
