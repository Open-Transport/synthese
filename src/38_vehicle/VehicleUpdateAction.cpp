
//////////////////////////////////////////////////////////////////////////
/// VehicleUpdateAction class implementation.
/// @file VehicleUpdateAction.cpp
/// @author Hugues Romain
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ActionException.h"
#include "ParametersMap.h"
#include "VehicleUpdateAction.hpp"
#include "Request.h"
#include "VehicleTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vehicle::VehicleUpdateAction>::FACTORY_KEY("VehicleUpdateAction");
	}

	namespace vehicle
	{
		const string VehicleUpdateAction::PARAMETER_VEHICLE_ID = Action_PARAMETER_PREFIX + "vi";
		const string VehicleUpdateAction::PARAMETER_ALLOWED_LINES = Action_PARAMETER_PREFIX + "al";
		const string VehicleUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string VehicleUpdateAction::PARAMETER_NUMBER = Action_PARAMETER_PREFIX + "nu";
		const string VehicleUpdateAction::PARAMETER_PICTURE = Action_PARAMETER_PREFIX + "pi";
		const string VehicleUpdateAction::PARAMETER_SEATS = Action_PARAMETER_PREFIX + "se";
		const string VehicleUpdateAction::PARAMETER_AVAILABLE = Action_PARAMETER_PREFIX + "av";
		const string VehicleUpdateAction::PARAMETER_URL = Action_PARAMETER_PREFIX + "ur";
		const string VehicleUpdateAction::PARAMETER_REGISTRATION_NUMBERS = Action_PARAMETER_PREFIX + "rn";



		ParametersMap VehicleUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_vehicle.get())
			{
				map.insert(PARAMETER_VEHICLE_ID, _vehicle->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_allowedLines)
			{
				map.insert(PARAMETER_ALLOWED_LINES, VehicleTableSync::SerializeAllowedLines(*_allowedLines));
			}
			if(_number)
			{
				map.insert(PARAMETER_NUMBER, *_number);
			}
			if(_seats)
			{
				map.insert(PARAMETER_SEATS, VehicleTableSync::SerializeSeats(*_seats));
			}
			if(_picture)
			{
				map.insert(PARAMETER_PICTURE, *_picture);
			}
			if(_available)
			{
				map.insert(PARAMETER_AVAILABLE, *_available);
			}
			if(_url)
			{
				map.insert(PARAMETER_URL, *_url);
			}
			if(_registrationNumbers)
			{
				map.insert(PARAMETER_REGISTRATION_NUMBERS, *_registrationNumbers);
			}
			return map;
		}



		void VehicleUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_VEHICLE_ID)) try
			{
				_vehicle = VehicleTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_VEHICLE_ID), *_env);
			}
			catch(ObjectNotFoundException<Vehicle>&)
			{
				throw ActionException("No such vehicle");
			}
			else
			{
				_vehicle.reset(new Vehicle);
			}

			if(map.isDefined(PARAMETER_ALLOWED_LINES))
			{
				_allowedLines = VehicleTableSync::UnserializeAllowedLines(map.get<string>(PARAMETER_ALLOWED_LINES), *_env);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			if(map.isDefined(PARAMETER_NUMBER))
			{
				_number = map.get<string>(PARAMETER_NUMBER);
			}

			if(map.isDefined(PARAMETER_PICTURE))
			{
				_picture = map.get<string>(PARAMETER_PICTURE);
			}

			if(map.isDefined(PARAMETER_SEATS))
			{
				_seats = VehicleTableSync::UnserializeSeats(map.get<string>(PARAMETER_SEATS));
			}

			if(map.isDefined(PARAMETER_AVAILABLE))
			{
				_available = map.get<bool>(PARAMETER_AVAILABLE);
			}

			if(map.isDefined(PARAMETER_URL))
			{
				_url = map.get<string>(PARAMETER_URL);
			}
			if(map.isDefined(PARAMETER_REGISTRATION_NUMBERS))
			{
				_registrationNumbers = map.get<string>(PARAMETER_REGISTRATION_NUMBERS);
			}
		}



		void VehicleUpdateAction::run(
			Request& request
		){
			//stringstream text;

			if(_name)
			{
				//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
				_vehicle->setName(*_name);
			}

			if(_number)
			{
				_vehicle->setNumber(*_number);
			}

			if(_picture)
			{
				_vehicle->setPicture(*_picture);
			}

			if(_seats)
			{
				_vehicle->setSeats(*_seats);
			}

			if(_allowedLines)
			{
				_vehicle->setAllowedLines(*_allowedLines);
			}

			if(_available)
			{
				_vehicle->setAvailable(*_available);
			}

			if(_url)
			{
				_vehicle->setURL(*_url);
			}
			if(_registrationNumbers)
			{
				_vehicle->setRegistrationNumbers(*_registrationNumbers);
			}

			VehicleTableSync::Save(_vehicle.get());

			// ::AddUpdateEntry(*_object, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_vehicle->getKey());
			}
		}



		bool VehicleUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
