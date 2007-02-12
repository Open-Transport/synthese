
/** CreateDisplayTypeAction class implementation.
	@file CreateDisplayTypeAction.cpp

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

#include "01_util/Conversion.h"

#include "11_interfaces/InterfaceModule.h"

#include "30_server/ActionException.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/CreateDisplayTypeAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	
	namespace departurestable
	{
		const string CreateDisplayTypeAction::PARAMETER_NAME = Action::PARAMETER_PREFIX + "dtc_name";
		const string CreateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action::PARAMETER_PREFIX + "dtc_interf";
		const string CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action::PARAMETER_PREFIX + "dtc_rows";


		Request::ParametersMap CreateDisplayTypeAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_INTERFACE_ID, Conversion::ToString(_interface->getKey())));
			map.insert(make_pair(PARAMETER_ROWS_NUMBER, Conversion::ToString(_rows_number)));
			return map;
		}

		void CreateDisplayTypeAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_NAME);
			if (it != map.end())
			{
				_name = it->second;
				map.erase(it);
				if (_name == "")
					throw ActionException("Name must be non empty");
			}

			it = map.find(PARAMETER_ROWS_NUMBER);
			if (it != map.end())
			{
				_rows_number = Conversion::ToInt(it->second);
				map.erase(it);
			}

			it = map.find(PARAMETER_INTERFACE_ID);
			if (it != map.end())
			{
				if (InterfaceModule::getInterfaces().contains(Conversion::ToLongLong(it->second)))
				{
					_interface = InterfaceModule::getInterfaces().get(Conversion::ToLongLong(it->second));
					map.erase(it);
				}
				else
				{
					map.erase(it);
					throw ActionException("Interface not found");
				}
			}
		}

		void CreateDisplayTypeAction::run()
		{
			DisplayType* dt = new DisplayType;
			dt->setName(_name);
			dt->setInterface(_interface);
			dt->setRowNumber(_rows_number);
			DisplayTypeTableSync::save(dt);
			delete dt;
		}
	}
}
