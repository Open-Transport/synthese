
//////////////////////////////////////////////////////////////////////////
/// HikingTrailUpdateAction class implementation.
/// @file HikingTrailUpdateAction.cpp
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

#include "HikingTrailUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "HikingRight.h"
#include "Request.h"
#include "HikingTrailTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, hiking::HikingTrailUpdateAction>::FACTORY_KEY("HikingTrailUpdateAction");
	}

	namespace hiking
	{

		const string HikingTrailUpdateAction::PARAMETER_DURATION = Action_PARAMETER_PREFIX + "du";
		const string HikingTrailUpdateAction::PARAMETER_MAP = Action_PARAMETER_PREFIX + "ma";
		const string HikingTrailUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string HikingTrailUpdateAction::PARAMETER_PROFILE = Action_PARAMETER_PREFIX + "pr";
		const string HikingTrailUpdateAction::PARAMETER_TRAIL_ID = Action_PARAMETER_PREFIX + "id";
		const string HikingTrailUpdateAction::PARAMETER_URL = Action_PARAMETER_PREFIX + "ur";



		ParametersMap HikingTrailUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_trail.get())
			{
				map.insert(PARAMETER_TRAIL_ID, _trail->getKey());
				map.insert(PARAMETER_DURATION, _duration);
				map.insert(PARAMETER_NAME, _name);
				map.insert(PARAMETER_MAP, _map);
				map.insert(PARAMETER_PROFILE, _profile);
				map.insert(PARAMETER_URL, _url);
			}
			return map;
		}



		void HikingTrailUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_trail = HikingTrailTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TRAIL_ID), *_env);
			}
			catch(ObjectNotFoundException<HikingTrail>&)
			{
				throw ActionException("No such trail");
			}
			_name = map.get<string>(PARAMETER_NAME);
			_map = map.get<string>(PARAMETER_MAP);
			_profile = map.get<string>(PARAMETER_PROFILE);
			_duration = map.get<string>(PARAMETER_DURATION);
			_url = map.get<string>(PARAMETER_URL);
		}



		void HikingTrailUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			_trail->setName(_name);
			_trail->setDuration(_duration);
			_trail->setMap(_map);
			_trail->setProfile(_profile);
			_trail->setURL(_url);

			HikingTrailTableSync::Save(_trail.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool HikingTrailUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<HikingRight>(WRITE);
		}
	}
}
