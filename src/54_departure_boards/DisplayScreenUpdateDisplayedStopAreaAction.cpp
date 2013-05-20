
//////////////////////////////////////////////////////////////////////////
/// DisplayScreenUpdateDisplayedStopAreaAction class implementation.
/// @file DisplayScreenUpdateDisplayedStopAreaAction.cpp
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

#include "DisplayScreenUpdateDisplayedStopAreaAction.hpp"

#include "ActionException.h"
#include "DisplayScreenTableSync.h"
#include "Profile.h"
#include "Session.h"
#include "StopAreaTableSync.hpp"
#include "User.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "GeographyModule.h"
#include "ArrivalDepartureTableLog.h"
#include "DBLogModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace pt;
	using namespace util;
	using namespace security;
	using namespace geography;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departure_boards::DisplayScreenUpdateDisplayedStopAreaAction>::FACTORY_KEY("DisplayScreenUpdateDisplayedStopAreaAction");
	}

	namespace departure_boards
	{
		const string DisplayScreenUpdateDisplayedStopAreaAction::PARAMETER_SCREEN_ID(
			Action_PARAMETER_PREFIX + "s"
			);
		const string DisplayScreenUpdateDisplayedStopAreaAction::PARAMETER_PLACE(
			Action_PARAMETER_PREFIX + "pl"
			);
		const string DisplayScreenUpdateDisplayedStopAreaAction::PARAMETER_CITY_NAME(
			Action_PARAMETER_PREFIX + "cn"
			);
		const string DisplayScreenUpdateDisplayedStopAreaAction::PARAMETER_PLACE_NAME = Action_PARAMETER_PREFIX + "pn";



		ParametersMap DisplayScreenUpdateDisplayedStopAreaAction::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get())
			{
				map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			}
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE, _place->getKey());
			}
			return map;
		}



		void DisplayScreenUpdateDisplayedStopAreaAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);

				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_PLACE, 0));
				if(id > 0)
				{
					_place = StopAreaTableSync::Get(id, *_env);
				}
				else
				{
					const string city(map.get<string>(PARAMETER_CITY_NAME));

					GeographyModule::CityList cities(GeographyModule::GuessCity(city, 1));
					if(cities.empty())
					{
						throw ActionException("City not found");
					}

					const string place(map.get<string>(PARAMETER_PLACE_NAME));
					vector<boost::shared_ptr<StopArea> > stops(
						cities.front()->search<StopArea>(place, 1)
					);
					if(stops.empty())
					{
						throw ActionException("Place not found");
					}
					_place = stops.front();
				}
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
		}



		void DisplayScreenUpdateDisplayedStopAreaAction::run(
			Request& request
		){
			// Comparison for log text generation
			stringstream log;
			DBLogModule::appendToLogIfChange(
				log,
				"Zone d'arrêt affichée",
				_screen->getDisplayedPlace() ? _screen->getDisplayedPlace()->getFullName() : "(non défini)",
				_place.get() ? _place->getFullName() : "(non défini)"
			);

			// Preparation of the action
			_screen->clearDisplayedPlaces();
			_screen->setDisplayedPlace(_place.get());

			// The action
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(*_screen, log.str(), *request.getUser());
		}



		bool DisplayScreenUpdateDisplayedStopAreaAction::isAuthorized(
			const Session* session
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocation() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocation()->getKey()));
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}
	}
}
