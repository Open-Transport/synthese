
//////////////////////////////////////////////////////////////////////////
/// VehicleServiceUpdateAction class implementation.
/// @file VehicleServiceUpdateAction.cpp
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
#include "VehicleServiceUpdateAction.hpp"
#include "Request.h"
#include "VehicleService.hpp"
#include "VehicleServiceTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_operation::VehicleServiceUpdateAction>::FACTORY_KEY("VehicleServiceUpdate");
	}

	namespace pt_operation
	{
		const string VehicleServiceUpdateAction::PARAMETER_VEHICLE_SERVICE_ID = Action_PARAMETER_PREFIX + "vehicle_service_id";
		const string VehicleServiceUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const string VehicleServiceUpdateAction::PARAMETER_FIELD_SERVICES = Action_PARAMETER_PREFIX + "_field_services";



		ParametersMap VehicleServiceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_vehicleService.get())
			{
				map.insert(PARAMETER_VEHICLE_SERVICE_ID, _vehicleService->getKey());
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}

			if(_services)
			{
				map.insert(PARAMETER_FIELD_SERVICES, VehicleServiceTableSync::SerializeServices(*_services));
			}
			return map;
		}



		void VehicleServiceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Vehicle service
			RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_VEHICLE_SERVICE_ID, 0));
			if(id > 0) try
			{
				_vehicleService = VehicleServiceTableSync::GetEditable(id, *_env);
			}
			catch(ObjectNotFoundException<VehicleService>&)
			{
				throw ActionException("No such vehicle service");
			}
			else
			{
				_vehicleService.reset(new VehicleService);
			}

			// Creator ID
			_setImportableUpdateFromParametersMap(*_env, map);

			// Name
			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			// Services
			if(map.isDefined(PARAMETER_FIELD_SERVICES))
			{
				_services = VehicleServiceTableSync::UnserializeServices(
					map.get<string>(PARAMETER_FIELD_SERVICES),
					*_env
				);
			}
		}



		void VehicleServiceUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			x::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(!_vehicleService.get())
			{
				_vehicleService.reset(new VehicleService);
			}

			// Importable
			_doImportableUpdate(*_vehicleService, request);

			if(_name)
			{
				_vehicleService->setName(*_name);
			}

			if(_services)
			{
				_vehicleService->setServices(*_services);
			}

			VehicleServiceTableSync::Save(_vehicleService.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_vehicleService->getKey());
			}

//			x::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool VehicleServiceUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<x>(x);
		}
	}
}

