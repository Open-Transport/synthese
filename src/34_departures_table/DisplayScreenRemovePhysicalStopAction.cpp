
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

#include "PhysicalStop.h"
#include "PhysicalStopTableSync.h"

#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

#include "DisplayScreenRemovePhysicalStopAction.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"

#include "Conversion.h"

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
				_screen = DisplayScreenTableSync::GetEditable(_request->getObjectId(), _env);

				uid id(map.getUid(PARAMETER_PHYSICAL, true, FACTORY_KEY));
				_stop = PhysicalStopTableSync::Get(id, _env);
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("Display screen not found" + e.getMessage());
			}
			catch (ObjectNotFoundException<PhysicalStop>& e)
			{
				throw ActionException("Specified stop not found" + e.getMessage());
			}
		}

		void DisplayScreenRemovePhysicalStopAction::run()
		{
			_screen->removePhysicalStop(_stop.get());
			DisplayScreenTableSync::Save(_screen.get());
		}
	}
}
