
//////////////////////////////////////////////////////////////////////////
/// HikingTrailStopAddAction class implementation.
/// @file HikingTrailStopAddAction.cpp
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

#include "HikingTrailStopAddAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "HikingRight.h"
#include "Request.h"
#include "HikingTrailTableSync.h"
#include "StopArea.hpp"
#include "City.h"
#include "GeographyModule.h"
#include "StopAreaTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<Action, hiking::HikingTrailStopAddAction>::FACTORY_KEY("HikingTrailStopAddAction");
	}

	namespace hiking
	{
		const string HikingTrailStopAddAction::PARAMETER_CITY = Action_PARAMETER_PREFIX + "ci";
		const string HikingTrailStopAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string HikingTrailStopAddAction::PARAMETER_RANK = Action_PARAMETER_PREFIX + "rk";
		const string HikingTrailStopAddAction::PARAMETER_TRAIL_ID = Action_PARAMETER_PREFIX + "id";



		ParametersMap HikingTrailStopAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_trail.get())
			{
				map.insert(PARAMETER_TRAIL_ID, _trail->getKey());
				if(_stop.get())
				{
					map.insert(PARAMETER_CITY, _stop->getCity()->getName());
					map.insert(PARAMETER_NAME, _stop->getName());
				}
				map.insert(PARAMETER_RANK, _rank.value_or(0));
			}
			return map;
		}



		void HikingTrailStopAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_trail = HikingTrailTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TRAIL_ID), *_env);
			}
			catch(ObjectNotFoundException<HikingTrail>&)
			{
				throw ActionException("No such trail");
			}

			const string city(map.get<string>(PARAMETER_CITY));

			GeographyModule::CityList cities(GeographyModule::GuessCity(city, 1));
			if(cities.empty())
			{
				throw ActionException("City not found");
			}

			const string place(map.get<string>(PARAMETER_NAME));
			vector<boost::shared_ptr<StopArea> > stops(
				cities.front()->search<StopArea>(place, 1)
			);
			if(stops.empty())
			{
				throw ActionException("Place not found");
			}
			_stop = StopAreaTableSync::GetEditable(stops.front()->getKey(), *_env);

			_rank = map.getOptional<size_t>(PARAMETER_RANK);
			if(_rank && *_rank > _trail->getStops().size())
			{
				throw ActionException("Rank is too high");
			}
		}



		void HikingTrailStopAddAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			HikingTrail::Stops stops(_trail->getStops());
			if(_rank)
			{
				stops.insert(stops.begin() + *_rank, _stop.get());
			}
			else
			{
				stops.push_back(_stop.get());
			}
			_trail->setStops(stops);

			HikingTrailTableSync::Save(_trail.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool HikingTrailStopAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<HikingRight>(WRITE);
		}
	}
}
