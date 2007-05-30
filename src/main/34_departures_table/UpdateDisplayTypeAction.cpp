
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
#include "30_server/Request.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/UpdateDisplayTypeAction.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace db;

	namespace departurestable
	{
		const string UpdateDisplayTypeAction::PARAMETER_ID = Action_PARAMETER_PREFIX + "dtu_id";
		const string UpdateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtu_name";
		const string UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "dtu_interf";
		const string UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtu_rows";


		ParametersMap UpdateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_dt.get())
				map.insert(make_pair(PARAMETER_ID, Conversion::ToString(_dt->getKey())));
			map.insert(make_pair(PARAMETER_NAME, _name));
			if (_interface.get())
				map.insert(make_pair(PARAMETER_INTERFACE_ID, Conversion::ToString(_interface->getKey())));
			map.insert(make_pair(PARAMETER_ROWS_NUMBER, Conversion::ToString(_rows_number)));
			return map;
		}

		void UpdateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_ID);
			if (it != map.end())
			{
				try
				{
					_dt = DisplayTypeTableSync::get(Conversion::ToLongLong(it->second));
				}
				catch (DBEmptyResultException<DisplayType>&)
				{
					throw ActionException("Display Type not found");
				}
			}

			it = map.find(PARAMETER_NAME);
			if (it != map.end())
			{
				_name = it->second;

				if (_name.empty())
					throw ActionException("Le nom ne peut être vide.");
				
				vector<shared_ptr<DisplayType> > v(DisplayTypeTableSync::search(_name, 0, 1));
				if (!v.empty())
					throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");
			}

			it = map.find(PARAMETER_ROWS_NUMBER);
			if (it != map.end())
			{
				_rows_number = Conversion::ToInt(it->second);

				if (_rows_number < 0)
					throw ActionException("Un nombre positif de lignes doit être choisi");
			}

			it = map.find(PARAMETER_INTERFACE_ID);
			if (it != map.end())
			{
				if (InterfaceModule::getInterfaces().contains(Conversion::ToLongLong(it->second)))
				{
					_interface = InterfaceModule::getInterfaces().get(Conversion::ToLongLong(it->second));
				}
				else
				{
					throw ActionException("Interface not found");
				}
			}
		}

		void UpdateDisplayTypeAction::run()
		{
			// Log entry content
			stringstream log;
			if (_dt->getName() != _name)
				log << " - Nom : " << _dt->getName() << " => " << _name;
			if (_dt->getInterface() != _interface)
				log << " - Interface : " << _dt->getInterface()->getName() << " => " << _interface->getName();
			if (_dt->getRowNumber() != _rows_number)
				log << " - Nombre de lignes : " << _dt->getRowNumber() <<  " => " << _rows_number;

			// Update
			_dt->setName(_name);
			_dt->setInterface(_interface);
			_dt->setRowNumber(_rows_number);
			DisplayTypeTableSync::save(_dt.get());

			// Log
			ArrivalDepartureTableLog::addUpdateTypeEntry(_dt, _request->getUser(), log.str());
		}
	}
}
