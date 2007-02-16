
/** AddForbiddenPlaceToDisplayScreen class implementation.
	@file AddForbiddenPlaceToDisplayScreen.cpp

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

#include "15_env/EnvModule.h"
#include "15_env/ConnectionPlace.h"

#include "30_server/ActionException.h"

#include "34_departures_table/AddForbiddenPlaceToDisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace env;
	
	namespace departurestable
	{
		const string AddForbiddenPlaceToDisplayScreen::PARAMETER_PLACE = Action_PARAMETER_PREFIX + "pla";


		Request::ParametersMap AddForbiddenPlaceToDisplayScreen::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_PLACE, _xxx));
			return map;
		}

		void AddForbiddenPlaceToDisplayScreen::setFromParametersMap(Request::ParametersMap& map)
		{
			try
			{
				_screen = DeparturesTableModule::getDisplayScreens().get(_request->getObjectId());

				Request::ParametersMap::iterator it;

				it = map.find(PARAMETER_PLACE);
				if (it == map.end())
					throw ActionException("Place not specified");
				
				_place = EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(it->second));
			}
			catch (DisplayScreen::RegistryKeyException e)
			{
				throw ActionException("Display screen not found");
			}
		}

		AddForbiddenPlaceToDisplayScreen::AddForbiddenPlaceToDisplayScreen()
			: Action()
			, _screen(NULL)
			, _place(NULL)
		{}

		void AddForbiddenPlaceToDisplayScreen::run()
		{
			_screen->addForbiddenPlace(_place);
			DisplayScreenTableSync::save(_screen);
		}
	}
}