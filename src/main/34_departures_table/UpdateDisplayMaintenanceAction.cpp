
/** UpdateDisplayMaintenanceAction class implementation.
	@file UpdateDisplayMaintenanceAction.cpp

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

#include "34_departures_table/UpdateDisplayMaintenanceAction.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	
	namespace departurestable
	{
		const string UpdateDisplayMaintenanceAction::PARAMETER_CONTROLS = Action_PARAMETER_PREFIX + "ctr";
		const string UpdateDisplayMaintenanceAction::PARAMETER_ONLINE = Action_PARAMETER_PREFIX + "oli";
		const string UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE = Action_PARAMETER_PREFIX + "mes";

		ParametersMap UpdateDisplayMaintenanceAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateDisplayMaintenanceAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(Request::PARAMETER_OBJECT_ID);
				if (it == map.end())
					throw ActionException("Display screen not specified");
				_displayScreen = DisplayScreenTableSync::get(Conversion::ToLongLong(it->second));

				it = map.find(PARAMETER_CONTROLS);
				if (it == map.end())
					throw ActionException("Controls per day not specified");
				_controls = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_ONLINE);
				if (it == map.end())
					throw ActionException("On line status not specified");
				_online = Conversion::ToBool(it->second);

				it = map.find(PARAMETER_MESSAGE);
				if (it == map.end())
					throw ActionException("Maintenance message not specified");
				_message = it->second;
			}
			catch (DBEmptyResultException<DisplayScreen>&)
			{
				throw ActionException("Specified display screen not found");
			}
		}

		void UpdateDisplayMaintenanceAction::run()
		{
			_displayScreen->setMaintenanceChecksPerDay(_controls);
			_displayScreen->setMaintenanceIsOnline(_online);
			_displayScreen->setMaintenanceMessage(_message);
			DisplayScreenTableSync::save(_displayScreen.get());
		}
	}
}
