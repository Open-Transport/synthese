
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

#include "DisplayScreenCPUCreateAction.h"

#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DeparturesTableModule.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace security;

	template<> const string FactorableTemplate<Action, departure_boards::DisplayScreenCPUCreateAction>::FACTORY_KEY("createdisplayscreenCPUAction");

	namespace departure_boards
	{
		const std::string DisplayScreenCPUCreateAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "pti";
		const string DisplayScreenCPUCreateAction::PARAMETER_LOCALIZATION_ID(Action_PARAMETER_PREFIX + "pli");

		ParametersMap DisplayScreenCPUCreateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_template.get()) map.insert(PARAMETER_TEMPLATE_ID, _template->getKey());
			map.insert(PARAMETER_LOCALIZATION_ID, _place.get() ? _place->getKey() : RegistryKeyType(0));
			return map;
		}

		void DisplayScreenCPUCreateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_TEMPLATE_ID))
			{
				_template = DisplayScreenCPUTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TEMPLATE_ID), *_env);
			}

			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LOCALIZATION_ID));
			if(id != 0) try
			{
				_place = StopAreaTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw ActionException("Specified localization not found");
			}
		}

		void DisplayScreenCPUCreateAction::run(Request& request)
		{
			// Preparation
			DisplayScreenCPU cpu;
			if (_template.get())
				cpu.copy(*_template);
			cpu.setPlace(_place.get());
			cpu.setIsOnline(true);

			// Action
			DisplayScreenCPUTableSync::Save(&cpu);

			// Request update
			request.setActionCreatedId(cpu.getKey());

			// Log
			ArrivalDepartureTableLog::addCreateEntry(cpu, *request.getUser());
		}



		bool DisplayScreenCPUCreateAction::isAuthorized(const Session* session
		) const {
			return
				_place.get() ?
				session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_place->getKey())) :
				session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE)
			;
		}
	}
}
