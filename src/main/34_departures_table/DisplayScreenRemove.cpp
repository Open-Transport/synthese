
/** DisplayScreenRemove class implementation.
	@file DisplayScreenRemove.cpp

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

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/DisplayScreenRemove.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"
#include "34_departures_table/ArrivalDepartureTableRight.h"

#include "12_security/Right.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	
	namespace departurestable
	{
		const string DisplayScreenRemove::PARAMETER_DISPLAY_SCREEN_ID(Action_PARAMETER_PREFIX + "dsi");


		ParametersMap DisplayScreenRemove::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_DISPLAY_SCREEN_ID, Conversion::ToString(_displayScreen->getKey())));
			return map;
		}

		void DisplayScreenRemove::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_DISPLAY_SCREEN_ID);
			if (it == map.end())
				throw ActionException("Display screen id not found");
			
			try
			{
				_displayScreen = DisplayScreenTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (...)
			{
				throw ActionException("Specified display screen not found");
			}
		}

		void DisplayScreenRemove::run()
		{
			DisplayScreenTableSync::remove(_displayScreen->getKey());

			ArrivalDepartureTableLog::addRemoveEntry(_displayScreen, _request->getUser());
		}

		bool DisplayScreenRemove::_isAuthorized() const
		{
			return _request->isAuthorized<ArrivalDepartureTableRight>(
				DELETE_RIGHT
				, FORBIDDEN
				, Conversion::ToString(_displayScreen->getKey())
				);
		}

		void DisplayScreenRemove::setDisplayScreen( boost::shared_ptr<const DisplayScreen> screen )
		{
			_displayScreen = screen;
		}
	}
}
