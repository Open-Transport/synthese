
/** DisplayScreenRemoveDisplayedPlaceAction class implementation.
	@file DisplayScreenRemoveDisplayedPlaceAction.cpp

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

#include "DisplayScreenRemoveDisplayedPlaceAction.h"

#include "34_departures_table/DisplayScreenTableSync.h"

#include "15_env/ConnectionPlaceTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, departurestable::DisplayScreenRemoveDisplayedPlaceAction>::FACTORY_KEY("dsrdp");
	
	namespace departurestable
	{
		const string DisplayScreenRemoveDisplayedPlaceAction::PARAMETER_PLACE = Action_PARAMETER_PREFIX + "pla";


		ParametersMap DisplayScreenRemoveDisplayedPlaceAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void DisplayScreenRemoveDisplayedPlaceAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(_request->getObjectId(), _env);

				uid id(map.getUid(PARAMETER_PLACE, true, FACTORY_KEY));
				_place = ConnectionPlaceTableSync::Get(id, _env);

			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
			catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>&)
			{
				throw ActionException("Specified place not found");
			}
		}

		void DisplayScreenRemoveDisplayedPlaceAction::run()
		{
			_screen->removeDisplayedPlace(_place.get());
			DisplayScreenTableSync::Save(_screen.get());
		}



		bool DisplayScreenRemoveDisplayedPlaceAction::_isAuthorized(

			) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
	}
}
