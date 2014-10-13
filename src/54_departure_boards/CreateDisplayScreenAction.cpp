
/** CreateDisplayScreenAction class implementation.
	@file CreateDisplayScreenAction.cpp

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

#include "CreateDisplayScreenAction.h"

#include "ActionException.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableModule.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenTableSync.h"
#include "Fetcher.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace security;
	using namespace geography;

	template<> const string FactorableTemplate<Action, departure_boards::CreateDisplayScreenAction>::FACTORY_KEY("createdisplayscreen");

	namespace departure_boards
	{
		const string CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "pti";
		const string CreateDisplayScreenAction::PARAMETER_LOCALIZATION_ID(Action_PARAMETER_PREFIX + "pli");
		const string CreateDisplayScreenAction::PARAMETER_CPU_ID(Action_PARAMETER_PREFIX + "cp");
		const string CreateDisplayScreenAction::PARAMETER_UP_ID(Action_PARAMETER_PREFIX + "up");
		const string CreateDisplayScreenAction::PARAMETER_SUB_SCREEN_TYPE(Action_PARAMETER_PREFIX + "st");
		const string CreateDisplayScreenAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "na");

		ParametersMap CreateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TEMPLATE_ID, _template ? _template->getKey() : RegistryKeyType(0));
			if(_up.get())
			{
				map.insert(PARAMETER_UP_ID, _up->getKey());
			}
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
			_name = map.getDefault<string>(PARAMETER_NAME);

			// Template
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID));
				if (id > 0)
				{
					_template = DisplayScreenTableSync::Get(id, *_env);
					if(_name.empty())
					{
						_name = "Copie de "+ _template->get<BroadCastPointComment>();
					}
				}
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("Template display screen not found "+ e.getMessage());
			}

			// Parent
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_UP_ID));
				if(id > 0)
				{
					_up = DisplayScreenTableSync::Get(id, *_env);
					_subScreenType = static_cast<DisplayScreen::SubScreenType>(map.getDefault<int>(PARAMETER_SUB_SCREEN_TYPE));
					setPlace(dynamic_cast<StopArea*>(&*_up->get<BroadCastPoint>()));
				}
				else
				{
					RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_CPU_ID));
					if (id > 0)
					{
						_cpu = DisplayScreenCPUTableSync::Get(id, *_env);
					}
					else
					{
						id = map.getDefault<RegistryKeyType>(PARAMETER_LOCALIZATION_ID);
						if (id > 0)
						{
							setPlace(
								Fetcher<NamedPlace>::Fetch(id, *_env).get()
							);
						}
				}	}
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("Parent display screen not found "+ e.getMessage());
			}
			catch (ObjectNotFoundException<DisplayScreenCPU>& e)
			{
				throw ActionException("Parent central unit not found "+ e.getMessage());
			}
			catch (ObjectNotFoundException<NamedPlace>& e)
			{
				throw ActionException("parent location not found "+ e.getMessage());
			}
		}



		void CreateDisplayScreenAction::run(Request& request)
		{
			// Preparation
			DisplayScreen screen;
			if (_template.get())
			{
				screen.copy(*_template);
			}
			if(_up.get())
			{
				screen.setParent(const_cast<DisplayScreen*>(_up.get()));
				screen.setSubScreenType(_subScreenType);
			}
			else if(_cpu.get())
			{
				screen.setRoot(const_cast<DisplayScreenCPU*>(_cpu.get()));
			}
			else
			{
				screen.setRoot(
					const_cast<PlaceWithDisplayBoards*>(_place.get())
				);
			}

			if(dynamic_cast<const StopArea*>(screen.getLocation()))
			{
				screen.set<BroadCastPoint>(*(static_cast<StopArea*>(const_cast<NamedPlace*>(screen.getLocation()))));
			}
			screen.set<BroadCastPointComment>(_name);
			screen.set<MaintenanceIsOnline>(true);

			// Action
			DisplayScreenTableSync::Save(&screen);

			// Request update
			request.setActionCreatedId(screen.getKey());

			// Log
			ArrivalDepartureTableLog::addCreateEntry(screen, *request.getUser());
		}



		bool CreateDisplayScreenAction::isAuthorized(const Session* session
		) const {
			if(!_place.get())
			{
				return false;
			}
			const NamedPlace* place(_place->getPlace());
			if(!place && _cpu.get())
			{
				place = _cpu->getPlace();
			}
			if(!place && _up.get())
			{
				place = _up->getLocation();
			}

			return
				place ?
				session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(place->getKey())) :
				session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE)
			;
		}



		void CreateDisplayScreenAction::setPlace( const geography::NamedPlace* value )
		{
			_place.reset(
				new PlaceWithDisplayBoards(
					value
			)	);
		}
}	}
