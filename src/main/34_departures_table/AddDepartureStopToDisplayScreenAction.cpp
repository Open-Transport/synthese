
/** AddDepartureStopToDisplayScreenAction class implementation.
	@file AddDepartureStopToDisplayScreenAction.cpp

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
#include "15_env/PhysicalStop.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/AddDepartureStopToDisplayScreenAction.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace env;
	
	namespace departurestable
	{
		const string AddDepartureStopToDisplayScreenAction::PARAMETER_STOP = Action_PARAMETER_PREFIX + "sto";


		ParametersMap AddDepartureStopToDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void AddDepartureStopToDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DeparturesTableModule::getDisplayScreens().get(_request->getObjectId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_STOP);
				if (it == map.end())
					throw ActionException("Place not specified");
				_stop = EnvModule::getPhysicalStops().get(Conversion::ToLongLong(it->second));
			}
			catch (DisplayScreen::RegistryKeyException e)
			{
				throw ActionException("Display screen not found");
			}
			catch (PhysicalStop::RegistryKeyException e)
			{
				throw ActionException("Specified stop not found");
			}
		}

		AddDepartureStopToDisplayScreenAction::AddDepartureStopToDisplayScreenAction()
			: Action()
			, _screen(NULL)
			, _stop(NULL)
		{}

		void AddDepartureStopToDisplayScreenAction::run()
		{
			_screen->addPhysicalStop(_stop);
			DisplayScreenTableSync::save(_screen);
		}
	}
}