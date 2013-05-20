
/** DisplayScreenAddDisplayedPlaceAction class implementation.
	@file DisplayScreenAddDisplayedPlaceAction.cpp

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

#include "DisplayScreenAddDisplayedPlaceAction.h"

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

	template<> const string util::FactorableTemplate<Action,departure_boards::DisplayScreenAddDisplayedPlaceAction>::FACTORY_KEY("dsadp");

	namespace departure_boards
	{
		const string DisplayScreenAddDisplayedPlaceAction::PARAMETER_SCREEN_ID(
			Action_PARAMETER_PREFIX + "s"
		);
		const string DisplayScreenAddDisplayedPlaceAction::PARAMETER_PLACE(
			Action_PARAMETER_PREFIX + "pl"
		);
		const string DisplayScreenAddDisplayedPlaceAction::PARAMETER_CITY_NAME(
			Action_PARAMETER_PREFIX + "cn"
		);
		const string DisplayScreenAddDisplayedPlaceAction::PARAMETER_PLACE_NAME = Action_PARAMETER_PREFIX + "pn";


		ParametersMap DisplayScreenAddDisplayedPlaceAction::getParametersMap() const
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

		void DisplayScreenAddDisplayedPlaceAction::_setFromParametersMap(const ParametersMap& map)
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

		void DisplayScreenAddDisplayedPlaceAction::run(Request& request)
		{
			_screen->addDisplayedPlace(_place.get());

			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Ajout de destination : "+ _place->getFullName(),
				*request.getUser()
			);
		}



		bool DisplayScreenAddDisplayedPlaceAction::isAuthorized(const Session* session
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


		void DisplayScreenAddDisplayedPlaceAction::setScreen(
			boost::shared_ptr<const DisplayScreen> value
		){
			_screen = const_pointer_cast<DisplayScreen>(value);
		}
	}
}
