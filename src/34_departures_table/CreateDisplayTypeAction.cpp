
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

#include "Conversion.h"

#include "Interface.h"
#include "InterfaceTableSync.h"

#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "CreateDisplayTypeAction.h"
#include "ArrivalDepartureTableLog.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace security;
	

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departurestable::CreateDisplayTypeAction>::FACTORY_KEY("createdisplaytype");
	}

	namespace departurestable
	{
		const string CreateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtc_name";
		const string CreateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "dtc_interf";
		const string CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtc_rows";


		ParametersMap CreateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			if (_interface.get())
				map.insert(PARAMETER_INTERFACE_ID, _interface->getKey());
			map.insert(PARAMETER_ROWS_NUMBER, _rows_number);
			return map;
		}

		void CreateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Name
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			if (_name.empty())
				throw ActionException("Le nom ne peut être vide.");
			Env env;
			DisplayTypeTableSync::Search(env, _name, UNKNOWN_VALUE, 0, 1);
			if (!env.getRegistry<DisplayType>().empty())
				throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");

			// Rows number
			_rows_number = map.getInt(PARAMETER_ROWS_NUMBER, true, FACTORY_KEY);
			if (_rows_number < 0)
				throw ActionException("Un nombre positif de lignes doit être choisi");

			// Interface
			try
			{
				_interface = InterfaceTableSync::Get(map.getUid(PARAMETER_INTERFACE_ID, true, FACTORY_KEY), _env);
			}
			catch (...)
			{
				throw ActionException("Interface not found");
			}
		}

		void CreateDisplayTypeAction::run()
		{
			DisplayType dt;
			dt.setName(_name);
			dt.setDisplayInterface(_interface.get());
			dt.setRowNumber(_rows_number);
			DisplayTypeTableSync::Save(&dt);

			// Log
			ArrivalDepartureTableLog::addCreateTypeEntry(&dt, _request->getUser().get());
		}



		bool CreateDisplayTypeAction::_isAuthorized(
		) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
	}
}
