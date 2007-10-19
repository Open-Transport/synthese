
/** DisplayScreenRemovePhysicalStopAction class implementation.
	@file DisplayScreenRemovePhysicalStopAction.cpp

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

#include "15_env/PhysicalStop.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

#include "34_departures_table/DisplayScreenRemovePhysicalStopAction.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"

#include "01_util/Conversion.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace env;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, departurestable::DisplayScreenRemovePhysicalStopAction>::FACTORY_KEY("dsrps");
	
	namespace departurestable
	{
		const string DisplayScreenRemovePhysicalStopAction::PARAMETER_PHYSICAL = Action_PARAMETER_PREFIX + "phy";


		ParametersMap DisplayScreenRemovePhysicalStopAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void DisplayScreenRemovePhysicalStopAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetUpdateable(_request->getObjectId());

				uid id(map.getUid(PARAMETER_PHYSICAL, true, FACTORY_KEY));
				_stop = PhysicalStop::Get(id);
			}
			catch (DisplayScreen::ObjectNotFoundException& e)
			{
				throw ActionException("Display screen not found" + e.getMessage());
			}
			catch (PhysicalStop::ObjectNotFoundException& e)
			{
				throw ActionException("Specified stop not found" + e.getMessage());
			}
		}

		void DisplayScreenRemovePhysicalStopAction::run()
		{
			_screen->removePhysicalStop(_stop.get());
			DisplayScreenTableSync::save(_screen.get());
		}
	}
}
