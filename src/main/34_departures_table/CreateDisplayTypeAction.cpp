
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

#include "11_interfaces/Interface.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/CreateDisplayTypeAction.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departurestable::CreateDisplayTypeAction>::FACTORY_KEY("createdisplaytype");
	}

	namespace departurestable
	{
		const string CreateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtc_name";
		const string CreateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "dtc_interf";
		const string CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtc_rows";
		const string CreateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER(Action_PARAMETER_PREFIX + "dtc_stops");


		ParametersMap CreateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			if (_interface.get())
				map.insert(make_pair(PARAMETER_INTERFACE_ID, Conversion::ToString(_interface->getKey())));
			map.insert(make_pair(PARAMETER_ROWS_NUMBER, Conversion::ToString(_rows_number)));
			map.insert(make_pair(PARAMETER_MAX_STOPS_NUMBER, Conversion::ToString(_max_stops_number)));
			return map;
		}

		void CreateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Name
			_name = Request::getStringFormParameterMap(map, PARAMETER_NAME, true, FACTORY_KEY);
			if (_name.empty())
				throw ActionException("Le nom ne peut être vide.");
			vector<shared_ptr<DisplayType> > v(DisplayTypeTableSync::search(_name, 0, 1));
			if (!v.empty())
				throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");

			// Rows number
			_rows_number = Request::getIntFromParameterMap(map, PARAMETER_ROWS_NUMBER, true, FACTORY_KEY);
			if (_rows_number < 0)
				throw ActionException("Un nombre positif de lignes doit être choisi");

			// Max stops number
			_max_stops_number = Request::getIntFromParameterMap(map, PARAMETER_MAX_STOPS_NUMBER, true, FACTORY_KEY);
			if (_max_stops_number < UNKNOWN_VALUE)
				throw ActionException("Un nombre positif d'arrêts intermédiaires lignes doit être choisi");

			// Interface
			uid id(Request::getUidFromParameterMap(map, PARAMETER_INTERFACE_ID, true, FACTORY_KEY));
			if (!Interface::Contains(id))
				throw ActionException("Interface not found");
			_interface = Interface::Get(id);
		}

		void CreateDisplayTypeAction::run()
		{
			shared_ptr<DisplayType> dt(new DisplayType);
			dt->setName(_name);
			dt->setInterface(_interface);
			dt->setRowNumber(_rows_number);
			dt->setMaxStopsNumber(_max_stops_number);
			DisplayTypeTableSync::save(dt.get());

			// Log
			ArrivalDepartureTableLog::addCreateTypeEntry(dt, _request->getUser());
		}
	}
}
