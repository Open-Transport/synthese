
//////////////////////////////////////////////////////////////////////////////////////////
///	VehicleInformationsService class implementation.
///	@file VehicleInformationsService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "VehicleInformationsService.hpp"

#include "Request.h"
#include "RequestException.h"
#include "StopPoint.hpp"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,vehicle::VehicleInformationsService>::FACTORY_KEY = "vehicle_informations";
	
	namespace vehicle
	{
		const string VehicleInformationsService::TAG_VEHICLE = "vehicle";
		const string VehicleInformationsService::TAG_POSITION = "position";
		const string VehicleInformationsService::ATTR_STOP_REQUESTED = "stop_requested";
		const string VehicleInformationsService::TAG_NEXT_STOP = "next_stop";
		


		ParametersMap VehicleInformationsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VehicleInformationsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Attempt to read vehicle id
			if(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
			{
				try
				{
					_vehicle = Env::GetOfficialEnv().get<Vehicle>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					);
				}
				catch(ObjectNotFoundException<Vehicle>&)
				{
					throw RequestException("No such vehicle");
				}
			}
			// Current vehicle if defined
			else if(!VehicleModule::GetCurrentVehiclePosition().getVehicle())
			{
				throw RequestException("A vehicle must be defined");
			}
		}



		ParametersMap VehicleInformationsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;
			
			// Informations about the vehicle
			if(_vehicle.get())
			{
				_vehicle->toParametersMap(map);
			}
			else if(VehicleModule::GetCurrentVehiclePosition().getVehicle())
			{
				VehicleModule::GetCurrentVehiclePosition().getVehicle()->toParametersMap(map);
			}

			// Position of the vehicle
			if(!_vehicle.get())
			{
				VehicleModule::GetCurrentVehiclePosition().toParametersMap(map);
			}

			// Stop requested
			map.insert(ATTR_STOP_REQUESTED, VehicleModule::GetStopRequested());

			// Next stops
			BOOST_FOREACH(const VehicleModule::NextStops::value_type& it, VehicleModule::GetNextStops())
			{
				shared_ptr<ParametersMap> stopPM(new ParametersMap);

				it->toParametersMap(*stopPM, true);

				map.insert(TAG_NEXT_STOP, stopPM);
			}
			return map;
		}
		
		
		
		bool VehicleInformationsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VehicleInformationsService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
