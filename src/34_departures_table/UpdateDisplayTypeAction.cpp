
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

#include "UpdateDisplayTypeAction.h"

#include "DisplayTypeTableSync.h"
#include "ArrivalDepartureTableLog.h"

#include "DBLogModule.h"

#include "Interface.h"
#include "InterfaceTableSync.h"

#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace db;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departurestable::UpdateDisplayTypeAction>::FACTORY_KEY("updatedisplaytype");
	}

	namespace departurestable
	{
		const string UpdateDisplayTypeAction::PARAMETER_ID = Action_PARAMETER_PREFIX + "dtu_id";
		const string UpdateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtu_name";
		const string UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "dtu_interf";
		const string UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtu_rows";
		const string UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER(Action_PARAMETER_PREFIX + "dtu_stops");

		ParametersMap UpdateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_dt.get())
				map.insert(PARAMETER_ID, _dt->getKey());
			map.insert(PARAMETER_NAME, _name);
			if (_interface.get())
				map.insert(PARAMETER_INTERFACE_ID, _interface->getKey());
			map.insert(PARAMETER_ROWS_NUMBER, _rows_number);
			map.insert(PARAMETER_MAX_STOPS_NUMBER, _max_stops_number);
			return map;
		}

		void UpdateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Display type
				_dt = DisplayTypeTableSync::GetEditable(map.getUid(PARAMETER_ID, true, FACTORY_KEY));

				// Name
				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
				if (_name != _dt->getName())
				{
					if (_name.empty())
						throw ActionException("Le nom ne peut être vide.");

					Env env;
					DisplayTypeTableSync::Search(env, _name, 0, 1);
					if (!env.template getRegistry<DisplayType>().empty())
						throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");
				}

				// Rows number
				_rows_number = map.getInt(PARAMETER_ROWS_NUMBER, true, FACTORY_KEY);
				if (_rows_number < 0)
					throw ActionException("Un nombre positif de lignes doit être choisi");

				// Interface
				_interface = InterfaceTableSync::Get(map.getUid(PARAMETER_INTERFACE_ID, true, FACTORY_KEY));

				// Max stops number
				_max_stops_number = map.getInt(PARAMETER_MAX_STOPS_NUMBER, true, FACTORY_KEY);
				if (_max_stops_number < UNKNOWN_VALUE)
					throw ActionException("Un nombre positif d'arrêts intermédiaires doit être choisi");
			}
			catch (ObjectNotFoundException<DisplayType>& e)
			{
				throw ActionException("Display Type not found / "+ e.getMessage());
			}
			catch(ObjectNotFoundException<Interface>& e)
			{
				throw ActionException("Interface not found / "+ e.getMessage());
			}
		}

		void UpdateDisplayTypeAction::run()
		{
			// Log entry content
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _dt->getName(), _name);
			DBLogModule::appendToLogIfChange(log, "Interface", _dt->getInterface()->getName(), _interface->getName());
			DBLogModule::appendToLogIfChange(log, "Nombre de lignes", _dt->getRowNumber(), _rows_number);
			DBLogModule::appendToLogIfChange(log, "Nombre d'arrêts intermédiaires", _dt->getMaxStopsNumber(), _max_stops_number);

			// Update
			_dt->setName(_name);
			_dt->setInterface(_interface.get());
			_dt->setRowNumber(_rows_number);
			_dt->setMaxStopsNumber(_max_stops_number);
			DisplayTypeTableSync::Save(_dt.get());

			// Log
			ArrivalDepartureTableLog::addUpdateTypeEntry(_dt.get(), _request->getUser().get(), log.str());
		}
	}
}
