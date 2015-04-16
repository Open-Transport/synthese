
//////////////////////////////////////////////////////////////////////////
/// HikingTrailStopRemoveAction class implementation.
/// @file HikingTrailStopRemoveAction.cpp
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

#include "HikingTrailStopRemoveAction.hpp"

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
		template<> const string FactorableTemplate<Action, hiking::HikingTrailStopRemoveAction>::FACTORY_KEY("HikingTrailStopRemoveAction");
	}

	namespace hiking
	{
		const string HikingTrailStopRemoveAction::PARAMETER_TRAIL_ID = Action_PARAMETER_PREFIX + "id";
		const string HikingTrailStopRemoveAction::PARAMETER_RANK = Action_PARAMETER_PREFIX + "rk";



		ParametersMap HikingTrailStopRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_trail.get())
			{
				map.insert(PARAMETER_TRAIL_ID, _trail->getKey());
				map.insert(PARAMETER_RANK, _rank);
			}
			return map;
		}



		void HikingTrailStopRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_trail = HikingTrailTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TRAIL_ID), *_env);
			}
			catch(ObjectNotFoundException<HikingTrail>&)
			{
				throw ActionException("No such trail");
			}
			_rank = map.get<size_t>(PARAMETER_RANK);

			if(_rank >= _trail->getStops().size())
			{
				throw ActionException("Rank is too high");
			}
		}



		void HikingTrailStopRemoveAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			HikingTrail::Stops stops(_trail->getStops());
			stops.erase(stops.begin() + _rank);
			_trail->setStops(stops);

			HikingTrailTableSync::Save(_trail.get());

			//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool HikingTrailStopRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<HikingRight>(WRITE);
		}
	}
}
