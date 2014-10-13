////////////////////////////////////////////////////////////////////////////////
/// UpdateDisplayScreenAction class implementation.
///	@file UpdateDisplayScreenAction.cpp
///	@author Hugues Romain
///	@date 2008-12-26 18:33
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "UpdateDisplayScreenAction.h"

#include "Profile.h"
#include "Session.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "User.h"
#include "ObjectNotFoundException.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DisplayScreen.h"
#include "DBLogModule.h"
#include "ArrivalDepartureTableRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace db;
	using namespace dblog;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departure_boards::UpdateDisplayScreenAction>::FACTORY_KEY("updatedisplayscreen");
	}

	namespace departure_boards
	{
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_SCREEN(Action_PARAMETER_PREFIX + "id");
		const string UpdateDisplayScreenAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "na");
		const string UpdateDisplayScreenAction::PARAMETER_WIRING_CODE(Action_PARAMETER_PREFIX + "wc");
		const string UpdateDisplayScreenAction::PARAMETER_TYPE(Action_PARAMETER_PREFIX + "ty");
		const string UpdateDisplayScreenAction::PARAMETER_COM_PORT(Action_PARAMETER_PREFIX + "cp");
		const string UpdateDisplayScreenAction::PARAMETER_CPU(Action_PARAMETER_PREFIX + "cu");
		const string UpdateDisplayScreenAction::PARAMETER_MAC_ADDRESS(Action_PARAMETER_PREFIX + "ma");
		const string UpdateDisplayScreenAction::PARAMETER_SUB_SCREEN_TYPE(Action_PARAMETER_PREFIX + "st");


		ParametersMap UpdateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			if (_screen.get() != NULL) map.insert(PARAMETER_DISPLAY_SCREEN, _screen->getKey());
			return map;
		}

		void UpdateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				setScreenId(map.get<RegistryKeyType>(PARAMETER_DISPLAY_SCREEN));

				_name = map.get<string>(PARAMETER_NAME);
				_wiringCode = map.get<int>(PARAMETER_WIRING_CODE);

				_type = DisplayTypeTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TYPE), *_env);

				_comPort = map.get<int>(PARAMETER_COM_PORT);

				_macAddress = map.getDefault<string>(PARAMETER_MAC_ADDRESS);

				optional<RegistryKeyType> id = map.getOptional<RegistryKeyType>(PARAMETER_CPU);
				if (id && *id>0)
				{
					_cpu = DisplayScreenCPUTableSync::Get(*id, *_env);
				}

				if(_screen->getParent())
				{
					_subScreenType = static_cast<DisplayScreen::SubScreenType>(map.get<int>(PARAMETER_SUB_SCREEN_TYPE));
				}
			}
			catch (ObjectNotFoundException<DisplayType>& e)
			{
				throw ActionException("display type", e, *this);
			}
			catch(ObjectNotFoundException<DisplayScreenCPU>& e)
			{
				throw ActionException("central unit", e, *this);
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw ActionException(e, *this);
			}
		}

		void UpdateDisplayScreenAction::run(Request& request)
		{
			// Comparison for log text generation
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _screen->get<BroadCastPointComment>(), _name);
			DBLogModule::appendToLogIfChange(log, "Code de branchement bus RS485", _screen->get<WiringCode>(), _wiringCode);
			DBLogModule::appendToLogIfChange(log, "Type de panneau", ((&*_screen->get<DisplayTypePtr>() != NULL) ? _screen->get<DisplayTypePtr>()->get<Name>() : string()), ((_type.get() != NULL) ? _type->get<Name>() : string()));
			DBLogModule::appendToLogIfChange(log, "Port COM", _screen->get<ComPort>(), _comPort);
			DBLogModule::appendToLogIfChange(log, "Unité centrale hôte", ((_screen->getRoot<DisplayScreenCPU>() != NULL) ? _screen->getRoot<DisplayScreenCPU>()->getName() : string()), ((_cpu.get() != NULL) ? _cpu->getName() : string()));
			DBLogModule::appendToLogIfChange(log, "Adresse MAC", _screen->get<MacAddress>(), _macAddress);
			if(_screen->getParent())
			{
				DBLogModule::appendToLogIfChange(log, "Rôle vis à vis du parent", DisplayScreen::GetSubScreenTypeLabel(_screen->getSubScreenType()), DisplayScreen::GetSubScreenTypeLabel(_subScreenType));
			}


			// Preparation of the action
			_screen->set<BroadCastPointComment>(_name);
			_screen->set<WiringCode>(_wiringCode);
			_screen->set<DisplayTypePtr>(*(const_cast<DisplayType*>(_type.get())));
			_screen->set<ComPort>(_comPort);
			_screen->setRoot(const_cast<DisplayScreenCPU*>(_cpu.get()));
			_screen->set<MacAddress>(_macAddress);
			if(_screen->getParent())
			{
				_screen->setSubScreenType(_subScreenType);
			}

			// The action
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(*_screen, log.str(), *request.getUser());
		}



		void UpdateDisplayScreenAction::setScreenId(
			util::RegistryKeyType id
		) {
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(id, *_env);
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("display screen", e, *this);
			}
		}



		bool UpdateDisplayScreenAction::isAuthorized(const Session* session
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocation() != NULL)
			{
				return session &&
					session->hasProfile() &&
					session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(
						WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocation()->getKey())
					);
			}
			else
			{
				return session &&
					session->hasProfile() &&
					session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}
	}
}
