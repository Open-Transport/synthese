
/** DisplayScreenCPUMaintenanceUpdateAction class implementation.
	@file DisplayScreenCPUCreateAction.cpp

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

#include "DisplayScreenCPUMaintenanceUpdateAction.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DeparturesTableModule.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayMaintenanceRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace security;
	using namespace dblog;

	template<> const string FactorableTemplate<Action, departurestable::DisplayScreenCPUMaintenanceUpdateAction>::FACTORY_KEY("DisplayScreenCPUMaintenanceUpdateAction");

	namespace departurestable
	{
		const std::string DisplayScreenCPUMaintenanceUpdateAction::PARAMETER_CPU = Action_PARAMETER_PREFIX + "cp";
		const std::string DisplayScreenCPUMaintenanceUpdateAction::PARAMETER_IS_ONLINE = Action_PARAMETER_PREFIX + "io";
		const std::string DisplayScreenCPUMaintenanceUpdateAction::PARAMETER_MAINTENANCE_MESSAGE = Action_PARAMETER_PREFIX + "mm";
		const std::string DisplayScreenCPUMaintenanceUpdateAction::PARAMETER_MONITORING_DELAY = Action_PARAMETER_PREFIX + "md";
		
		ParametersMap DisplayScreenCPUMaintenanceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_cpu.get())
			{
				map.insert(PARAMETER_CPU, _cpu->getKey());
			}
			return map;
		}

		void DisplayScreenCPUMaintenanceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			setCPU(map.getUid(PARAMETER_CPU, true, FACTORY_KEY));
			_isOnline = map.getBool(PARAMETER_IS_ONLINE, true, false, FACTORY_KEY);
			_maintenanceMessage = map.getString(PARAMETER_MAINTENANCE_MESSAGE, true, FACTORY_KEY);
			_monitoringDelay = minutes(map.getInt(PARAMETER_MONITORING_DELAY, true, FACTORY_KEY));
		}

		void DisplayScreenCPUMaintenanceUpdateAction::run()
		{
			// Log
			DisplayMaintenanceLog::AddAdminEntry(*_cpu, *_request->getUser(), "Statut en service", _cpu->getIsOnline() ? "OUI" : "NON", _isOnline ? "OUI" : "NON");
			DisplayMaintenanceLog::AddAdminEntry(*_cpu, *_request->getUser(), "Message de maintenance", _cpu->getMaintenanceMessage(), _maintenanceMessage);
			DisplayMaintenanceLog::AddAdminEntry(*_cpu, *_request->getUser(), "Délai de contact", to_simple_string(_cpu->getMonitoringDelay()), to_simple_string(_monitoringDelay));

			// Action
			_cpu->setIsOnline(_isOnline);
			_cpu->setMaintenanceMessage(_maintenanceMessage);
			_cpu->setMonitoringDelay(_monitoringDelay);
			DisplayScreenCPUTableSync::Save(_cpu.get());
		}



		bool DisplayScreenCPUMaintenanceUpdateAction::_isAuthorized(
		) const {
			return
				_request->isAuthorized<DisplayMaintenanceRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_cpu->getPlace()->getKey()))
			;
		}

		void DisplayScreenCPUMaintenanceUpdateAction::setCPU( util::RegistryKeyType id )
		{
			try
			{
				_cpu = DisplayScreenCPUTableSync::GetEditable(id, _env);
			}
			catch (...)
			{
				throw ActionException("Specified CPU not found");
			}
		}
	}
}
