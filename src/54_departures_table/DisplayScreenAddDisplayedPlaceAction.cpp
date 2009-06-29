
/** DisplayScreenAddDisplayedPlaceAction class implementation.
	@file DisplayScreenAddDisplayedPlaceAction.cpp

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

#include "DisplayScreenAddDisplayedPlaceAction.h"
#include "DisplayScreenTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "GeographyModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace security;
	using namespace geography;

	template<> const string util::FactorableTemplate<Action,departurestable::DisplayScreenAddDisplayedPlaceAction>::FACTORY_KEY("dsadp");
	
	namespace departurestable
	{
		const string DisplayScreenAddDisplayedPlaceAction::PARAMETER_PLACE = Action_PARAMETER_PREFIX + "pl";
		const string DisplayScreenAddDisplayedPlaceAction::PARAMETER_CITY_NAME = Action_PARAMETER_PREFIX + "cn";
		const string DisplayScreenAddDisplayedPlaceAction::PARAMETER_PLACE_NAME = Action_PARAMETER_PREFIX + "pn";


		ParametersMap DisplayScreenAddDisplayedPlaceAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void DisplayScreenAddDisplayedPlaceAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(_request->getObjectId(), *_env);

				uid id(map.getUid(PARAMETER_PLACE, false, FACTORY_KEY));

				if(id > 0)
				{
					_placeSptr = ConnectionPlaceTableSync::Get(id, *_env);
					_place = _placeSptr.get();
				}
				else
				{
					const string city(map.getString(PARAMETER_CITY_NAME, true, FACTORY_KEY));
					
					GeographyModule::CityList cities(GeographyModule::GuessCity(city, 1));
					if(cities.empty())
					{
						throw ActionException("City not found");
					}

					const string place(map.getString(PARAMETER_PLACE_NAME, true, FACTORY_KEY));
					vector<const PublicTransportStopZoneConnectionPlace*> stops(
						cities.front()->search<PublicTransportStopZoneConnectionPlace>(place, 1)
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

		void DisplayScreenAddDisplayedPlaceAction::run()
		{
			_screen->addDisplayedPlace(_place);
			DisplayScreenTableSync::Save(_screen.get());
		}



		bool DisplayScreenAddDisplayedPlaceAction::_isAuthorized(
		) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
	}
}
