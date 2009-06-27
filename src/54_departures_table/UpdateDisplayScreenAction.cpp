////////////////////////////////////////////////////////////////////////////////
/// UpdateDisplayScreenAction class implementation.
///	@file UpdateDisplayScreenAction.cpp
///	@author Hugues Romain
///	@date 2008-12-26 18:33
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "ObjectNotFoundException.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "UpdateDisplayScreenAction.h"
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
	using namespace env;
	using namespace db;
	using namespace dblog;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departurestable::UpdateDisplayScreenAction>::FACTORY_KEY("updatedisplayscreen");
	}

	namespace departurestable
	{
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_SCREEN(Action_PARAMETER_PREFIX + "id");
		const string UpdateDisplayScreenAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "na");
		const string UpdateDisplayScreenAction::PARAMETER_WIRING_CODE(Action_PARAMETER_PREFIX + "wc");
		const string UpdateDisplayScreenAction::PARAMETER_TYPE(Action_PARAMETER_PREFIX + "ty");
		const string UpdateDisplayScreenAction::PARAMETER_COM_PORT(Action_PARAMETER_PREFIX + "cp");
		const string UpdateDisplayScreenAction::PARAMETER_CPU(Action_PARAMETER_PREFIX + "cu");
		const string UpdateDisplayScreenAction::PARAMETER_MAC_ADDRESS(Action_PARAMETER_PREFIX + "ma");
		

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
				
				_type = DisplayTypeTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TYPE), _env);

				_comPort = map.get<int>(PARAMETER_COM_PORT);

				_macAddress = map.getDefault<string>(PARAMETER_MAC_ADDRESS);

				optional<RegistryKeyType> id = map.getOptional<RegistryKeyType>(PARAMETER_CPU);
				if (id)
				{
					_cpu = DisplayScreenCPUTableSync::Get(*id, _env);
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

		void UpdateDisplayScreenAction::run()
		{
			// Comparison for log text generation
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _screen->getLocalizationComment(), _name);
			DBLogModule::appendToLogIfChange(log, "Code de branchement bus RS485", _screen->getWiringCode(), _wiringCode);
			DBLogModule::appendToLogIfChange(log, "Type de panneau", ((_screen->getType() != NULL) ? _screen->getType()->getName() : string()), ((_type.get() != NULL) ? _type->getName() : string()));
			DBLogModule::appendToLogIfChange(log, "Port COM", _screen->getComPort(), _comPort);
			DBLogModule::appendToLogIfChange(log, "Unité centrale hôte", ((_screen->getCPU() != NULL) ? _screen->getCPU()->getName() : string()), ((_cpu.get() != NULL) ? _cpu->getName() : string()));
			DBLogModule::appendToLogIfChange(log, "Adresse MAC", _screen->getMacAddress(), _macAddress);


			// Preparation of the action
			_screen->setLocalizationComment(_name);
			_screen->setWiringCode(_wiringCode);
			_screen->setType(_type.get());
			_screen->setComPort(_comPort);
			_screen->setCPU(_cpu.get());
			_screen->setMacAddress(_macAddress);

			// The action
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(*_screen, log.str(), *_request->getUser());
		}



		void UpdateDisplayScreenAction::setScreenId(
			util::RegistryKeyType id
		) {
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(id, _env);
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("display screen", e, *this);
			}
		}



		bool UpdateDisplayScreenAction::_isAuthorized(
		) const {
			if (_screen->getLocalization() != NULL)
			{
				return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_screen->getLocalization()->getKey()));
			}
			else
			{
				return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}
	}
}
