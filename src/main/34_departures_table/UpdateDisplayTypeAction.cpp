
/** UpdateDisplayTypeAction class implementation.
@file UpdateDisplayTypeAction.cpp

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

#include "02_db/DBEmptyResultException.h"

#include "11_interfaces/InterfaceModule.h"

#include "30_server/ActionException.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/UpdateDisplayTypeAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace db;

	namespace departurestable
	{
		const string UpdateDisplayTypeAction::PARAMETER_ID = Action::PARAMETER_PREFIX + "dtu_id";
		const string UpdateDisplayTypeAction::PARAMETER_NAME = Action::PARAMETER_PREFIX + "dtu_name";
		const string UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action::PARAMETER_PREFIX + "dtu_interf";
		const string UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action::PARAMETER_PREFIX + "dtu_rows";


		Request::ParametersMap UpdateDisplayTypeAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_ID, Conversion::ToString(_dt->getKey())));
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_INTERFACE_ID, Conversion::ToString(_interface->getKey())));
			map.insert(make_pair(PARAMETER_ROWS_NUMBER, Conversion::ToString(_rows_number)));
			return map;
		}

		void UpdateDisplayTypeAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_ID);
			if (it != map.end())
			{
				try
				{
					_dt = DisplayTypeTableSync::get(Conversion::ToLongLong(it->second));
				}
				catch (DBEmptyResultException e)
				{
					map.erase(it);
					throw ActionException("Display Type not found");
				}
				map.erase(it);
			}

			it = map.find(PARAMETER_NAME);
			if (it != map.end())
			{
				_name = it->second;
				map.erase(it);
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

		void UpdateDisplayTypeAction::run()
		{
			_dt->setName(_name);
			_dt->setInterface(_interface);
			_dt->setRowNumber(_rows_number);
			DisplayTypeTableSync::save(_dt);
		}

		UpdateDisplayTypeAction::~UpdateDisplayTypeAction()
		{
			delete _dt;
		}

		UpdateDisplayTypeAction::UpdateDisplayTypeAction()
			: _dt(NULL), _interface(NULL)
		{
		}
	}
}