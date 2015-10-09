
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
		const string VehicleInformationsService::TAG_VEHICLE_INFORMATIONS = "vehicle_informations";
		const string VehicleInformationsService::TAG_VEHICLE = "vehicle";
		const string VehicleInformationsService::TAG_POSITION = "position";
		const string VehicleInformationsService::TAG_JOURNEY = "journey";
		const string VehicleInformationsService::TAG_IGNITION = "ignition";
		const string VehicleInformationsService::TAG_SCREEN = "screen";
		const string VehicleInformationsService::TAG_SCREENS = "screens";
		const string VehicleInformationsService::TAG_EXTRA_PARAMETERS = "extra_parameters";

		server::FunctionAPI VehicleInformationsService::getAPI() const
		{
			FunctionAPI api(
				"38_vehicle",
				"Query vehicle information",
				"Return many information about the vehicle associated to this SYNTHESE instance"
				"or from a given vehicle specified by its 'roid'\n"
				"The information is returned as a parameters map."
			);
			api.addParams(Request::PARAMETER_OBJECT_ID, "The 'roid' of a Vehicle", false);
			api.addParams(Vehicle::PARAMETER_SRID, "The 'srid' of the coordinates system so use", false);
			return api;
		}

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

			// SRID or 0 if none
			_srid = map.getDefault<CoordinatesSystem::SRID>(Vehicle::PARAMETER_SRID,0);
			
			Function::setOutputFormatFromMap(map,string());
		}



		ParametersMap VehicleInformationsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;

			map.insert(Vehicle::PARAMETER_SRID, static_cast<int>(_srid));
			
			// Informations about the vehicle
			if(_vehicle.get())
			{
				_vehicle->toParametersMap(map, true);
			}
			else if(VehicleModule::GetCurrentVehiclePosition().getVehicle())
			{
				VehicleModule::GetCurrentVehiclePosition().getVehicle()->toParametersMap(map, true);
			}

			// Position of the vehicle
			if(!_vehicle.get())
			{
				// Current vehicle position
				VehicleModule::GetCurrentVehiclePosition().toParametersMap(map, true);

				// Journey
				VehicleModule::GetCurrentJourney().toParametersMap(map);

				// Ignition
				map.insert(TAG_IGNITION, VehicleModule::getIgnition());

				// Screens
				boost::shared_ptr<ParametersMap> screenMap(new ParametersMap);
				BOOST_FOREACH(const VehicleModule::VehicleScreensMap::value_type& item, VehicleModule::GetVehicleScreens())
				{
					// Insert a submap for each screen
					boost::shared_ptr<ParametersMap> subScreenMap(new ParametersMap);
					item.second.toParametersMap(*subScreenMap, true);
					screenMap->insert(TAG_SCREEN, subScreenMap);
				}
				map.insert(TAG_SCREENS, screenMap);

				// Extra Parameters
				boost::shared_ptr<ParametersMap> extraParamMap(new ParametersMap);
				BOOST_FOREACH(const VehicleModule::ExtraParameterMap::value_type& item, VehicleModule::GetExtraParameters())
				{
					extraParamMap->insert(item.first, item.second);
				}
				map.insert(TAG_EXTRA_PARAMETERS, extraParamMap);
			}
			
			if (_outputFormat == MimeTypes::JSON)
			{
				map.outputJSON(stream, TAG_VEHICLE_INFORMATIONS);
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
			return getOutputMimeTypeFromOutputFormat();
		}
}	}
