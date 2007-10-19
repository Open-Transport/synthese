
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

#include "UpdateDisplayMaintenanceAction.h"

#include "34_departures_table/DisplayScreenTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/QueryString.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, departurestable::UpdateDisplayMaintenanceAction>::FACTORY_KEY("udm");

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
				uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
				_displayScreen = DisplayScreenTableSync::GetUpdateable(id);

				_controls = map.getInt(PARAMETER_CONTROLS, true, FACTORY_KEY);
				_online = map.getBool(PARAMETER_ONLINE, true, false, FACTORY_KEY);
				_message = map.getString(PARAMETER_MESSAGE, true, FACTORY_KEY);
			}
			catch (DisplayScreen::ObjectNotFoundException&)
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
