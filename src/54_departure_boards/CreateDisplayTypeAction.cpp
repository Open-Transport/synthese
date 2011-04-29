
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
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace security;


	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departure_boards::CreateDisplayTypeAction>::FACTORY_KEY("createdisplaytype");
	}

	namespace departure_boards
	{
		const string CreateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string CreateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "di";
		const string CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "ro";
		const string CreateDisplayTypeAction::PARAMETER_MONITORING_INTERFACE_ID(
			Action_PARAMETER_PREFIX + "mi"
		);



		ParametersMap CreateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			if (_interface.get())
				map.insert(PARAMETER_INTERFACE_ID, _interface->getKey());
			if(_monitoringInterface.get())
				map.insert(PARAMETER_MONITORING_INTERFACE_ID, _monitoringInterface->getKey());
			map.insert(PARAMETER_ROWS_NUMBER, _rows_number);
			return map;
		}



		void CreateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Name
			_name = map.get<string>(PARAMETER_NAME);
			if (_name.empty())
				throw ActionException("Le nom ne peut être vide.");
			Env env;
			DisplayTypeTableSync::Search(env, _name, optional<RegistryKeyType>(), 0, 1);
			if (!env.getRegistry<DisplayType>().empty())
				throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");

			// Rows number
			_rows_number = map.get<int>(PARAMETER_ROWS_NUMBER);
			if (_rows_number < 0)
				throw ActionException("Un nombre positif de lignes doit être choisi");

			// Interface
			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_INTERFACE_ID));
			if(id)
			try
			{
				_interface = InterfaceTableSync::Get(*id, *_env);
			}
			catch (ObjectNotFoundException<Interface>& e)
			{
				throw ActionException("Interface d'affichage", e, *this);
			}

			// Monitoring Interface
			id = map.getOptional<RegistryKeyType>(PARAMETER_MONITORING_INTERFACE_ID);
			if(id && *id > 0)
			try
			{
				_monitoringInterface = InterfaceTableSync::Get(*id, *_env);
			}
			catch (ObjectNotFoundException<Interface>& e)
			{
				throw ActionException("Interface de supervision", e, *this);
			}
		}

		void CreateDisplayTypeAction::run(Request& request)
		{
			DisplayType dt;
			dt.setName(_name);
			dt.setDisplayInterface(_interface.get());
			dt.setMonitoringInterface(_monitoringInterface.get());
			dt.setRowNumber(_rows_number);
			DisplayTypeTableSync::Save(&dt);

			// Request update
			request.setActionCreatedId(dt.getKey());

			// Log
			ArrivalDepartureTableLog::addCreateEntry(dt, *request.getUser());
		}



		bool CreateDisplayTypeAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
	}
}
