
/** DisplayScreenCPUCreateAction class implementation.
	@file DisplayScreenCPUCreateAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DisplayScreenCPUUpdateAction.h"

#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "DeparturesTableModule.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "DBLogModule.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace security;
	using namespace dblog;

	template<> const string FactorableTemplate<Action, departure_boards::DisplayScreenCPUUpdateAction>::FACTORY_KEY("DisplayScreenCPUUpdateAction");

	namespace departure_boards
	{
		const std::string DisplayScreenCPUUpdateAction::PARAMETER_CPU = Action_PARAMETER_PREFIX + "cp";
		const std::string DisplayScreenCPUUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const std::string DisplayScreenCPUUpdateAction::PARAMETER_MAC_ADDRESS = Action_PARAMETER_PREFIX + "ma";

		ParametersMap DisplayScreenCPUUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_MAC_ADDRESS, _macAddress);
			if(_cpu.get())
			{
				map.insert(PARAMETER_CPU, _cpu->getKey());
			}
			return map;
		}

		void DisplayScreenCPUUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			setCPU(map.get<RegistryKeyType>(PARAMETER_CPU));
			_name = map.get<string>(PARAMETER_NAME);
			_macAddress = map.get<string>(PARAMETER_MAC_ADDRESS);
		}

		void DisplayScreenCPUUpdateAction::run(Request& request)
		{
			// Log
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _cpu->getName(), _name);
			DBLogModule::appendToLogIfChange(log, "Adresse MAC", _cpu->getMacAddress(), _macAddress);


			// Action
			_cpu->setName(_name);
			_cpu->setMacAddress(_macAddress);
			DisplayScreenCPUTableSync::Save(_cpu.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(*_cpu, log.str(), *request.getUser());
		}



		bool DisplayScreenCPUUpdateAction::isAuthorized(const Session* session
		) const {
			return
				_cpu->getPlace() ?
				session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_cpu->getPlace()->getKey())) :
				session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER)
			;
		}

		void DisplayScreenCPUUpdateAction::setCPU( util::RegistryKeyType id )
		{
			try
			{
				_cpu = DisplayScreenCPUTableSync::GetEditable(id, *_env);
			}
			catch (...)
			{
				throw ActionException("Specified CPU not found");
			}
		}
	}
}
