
/** CreateDisplayScreenAction class implementation.
	@file CreateDisplayScreenAction.cpp

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

#include "CreateDisplayScreenAction.h"

#include "DisplayScreenTableSync.h"
#include "DeparturesTableModule.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "Conversion.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace security;
	
	template<> const string FactorableTemplate<Action, departurestable::CreateDisplayScreenAction>::FACTORY_KEY("createdisplayscreen");

	namespace departurestable
	{
		const std::string CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "pti";
		const string CreateDisplayScreenAction::PARAMETER_LOCALIZATION_ID(Action_PARAMETER_PREFIX + "pli");
		const string CreateDisplayScreenAction::PARAMETER_CPU_ID(Action_PARAMETER_PREFIX + "cp");

		ParametersMap CreateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TEMPLATE_ID, _template ? _template->getKey() : uid(0));
			if(_cpu.get())
			{
				map.insert(PARAMETER_CPU_ID, _cpu->getKey());
			}
			else if(_place.get())
			{
				map.insert(PARAMETER_LOCALIZATION_ID, _place->getKey());
			}
			return map;
		}

		void CreateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_TEMPLATE_ID, false, FACTORY_KEY));
			if (id > 0)
			{
				_template = DisplayScreenTableSync::Get(id, *_env);
			}

			id = map.getUid(PARAMETER_CPU_ID, false, FACTORY_KEY);
			if (id > 0)
			{
				setCPU(id);
			}
			else
			{
				id = map.getUid(PARAMETER_LOCALIZATION_ID, false, FACTORY_KEY);
				if (id > 0)
				{
					setPlace(id);
				}
			}
		}

		void CreateDisplayScreenAction::run(Request& request)
		{
			// Preparation
			DisplayScreen screen;
			if (_template.get())
				screen.copy(_template.get());
			screen.setLocalization(_place.get());
			if(_cpu.get())
			{
				screen.setCPU(_cpu.get());
			}
			screen.setMaintenanceIsOnline(true);

			// Action
			DisplayScreenTableSync::Save(&screen);

			// Request update
			request.setActionCreatedId(screen.getKey());

			// Log
			ArrivalDepartureTableLog::addCreateEntry(screen, *request.getUser());
		}

		void CreateDisplayScreenAction::setPlace(RegistryKeyType id)
		{
			if(id <= 0) return;
			try
			{
				_place = ConnectionPlaceTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw ActionException("Specified localization not found");
			}
		}



		bool CreateDisplayScreenAction::isAuthorized(const Profile& profile
		) const {
			return
				_place.get() ?
				profile.isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_place->getKey())) :
				profile.isAuthorized<ArrivalDepartureTableRight>(WRITE)
			;
		}

		void CreateDisplayScreenAction::setCPU( util::RegistryKeyType id )
		{
			if(id <= 0) return;
			try
			{
				_cpu = DisplayScreenCPUTableSync::Get(id, *_env);
				if(_cpu->getPlace())
				{
					setPlace(_cpu->getPlace()->getKey());
				}
			}
			catch (...)
			{
				throw ActionException("Specified CPU not found");
			}
		}
	}
}
