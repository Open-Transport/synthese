
/** VehicleModule class implementation.
    @file VehicleModule.cpp

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

#include "VehicleModule.hpp"

#include "RollingStockTableSync.hpp"
#include "ScheduledService.h"
#include "ServiceComposition.hpp"
#include "Vehicle.hpp"
#include "VehiclePositionTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace vehicle;
	
	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass, VehicleModule>::FACTORY_KEY = "38_vehicle";
	}

	namespace vehicle
	{
		VehicleModule::LinesAllowedVehicles VehicleModule::_linesAllowedVehicles;
		VehicleModule::ServiceCompositions VehicleModule::_serviceCompositions;
		VehiclePosition VehicleModule::_currentVehiclePosition;
		CurrentJourney VehicleModule::_currentJourney;
		VehicleModule::VehicleScreensMap VehicleModule::_currentScreens;
		VehicleModule::ExtraParameterMap VehicleModule::_extraParameters;
		bool VehicleModule::_ignition(false);
		const string VehicleModule::MODULE_PARAM_CURRENT_VEHICLE_ID = "current_vehicle_id";
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<VehicleModule>::NAME = "Vehicule";

		template<> void ModuleClassTemplate<VehicleModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<VehicleModule>::Init()
		{
			// Creation of each transport mode corresponding to Trident values except "Other" which is used for null pointer
			Env env;
			vector<string> tridentKeys;
			tridentKeys.push_back("Air");
			tridentKeys.push_back("Train");
			tridentKeys.push_back("LongDistanceTrain");
			tridentKeys.push_back("LocalTrain");
			tridentKeys.push_back("RapidTransit");
			tridentKeys.push_back("Metro");
			tridentKeys.push_back("Tramway");
			tridentKeys.push_back("Coach");
			tridentKeys.push_back("Bus");
			tridentKeys.push_back("Ferry");
			tridentKeys.push_back("Waterborne");
			tridentKeys.push_back("PrivateVehicle");
			tridentKeys.push_back("Walk");
			tridentKeys.push_back("Trolleybus");
			tridentKeys.push_back("Bicycle");
			tridentKeys.push_back("Shuttle");
			tridentKeys.push_back("Taxi");
			tridentKeys.push_back("VAL");

			BOOST_FOREACH(const string& tridentKey, tridentKeys)
			{
				RollingStockTableSync::SearchResult rollingStocks(RollingStockTableSync::Search(env, tridentKey, true));
				if(rollingStocks.empty())
				{
					RollingStock s;
					s.set<Name>(tridentKey);
					s.set<TridentKey>(tridentKey);
					s.set<IsTridentReference>(true);
					RollingStockTableSync::Save(&s);
				}
			}

			// In the init section in order to read this parameter after the data load (DBModule::Init)
			RegisterParameter(VehicleModule::MODULE_PARAM_CURRENT_VEHICLE_ID, "", &VehicleModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<VehicleModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<VehicleModule>::End()
		{
		}



		template<> void ModuleClassTemplate<VehicleModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<VehicleModule>::CloseThread(
			
			){
		}
	}

	namespace vehicle
	{
		void VehicleModule::RegisterVehicle(const Vehicle& vehicle )
		{
			BOOST_FOREACH(const AllowedLines::Type::value_type& line, vehicle.get<AllowedLines>())
			{
				_linesAllowedVehicles[line].insert(&vehicle);
			}
		}



		void VehicleModule::UnregisterVehicle(const Vehicle& vehicle )
		{
			BOOST_FOREACH(const AllowedLines::Type::value_type& line, vehicle.get<AllowedLines>())
			{
				_linesAllowedVehicles[line].erase(&vehicle);
			}
		}



		void VehicleModule::RegisterComposition(const ServiceComposition& composition )
		{
			_serviceCompositions[composition.getService()].insert(&composition);
		}



		void VehicleModule::UnregisterComposition(const ServiceComposition& composition )
		{
			_serviceCompositions[composition.getService()].erase(&composition);
		}



		VehicleModule::ServiceCompositions::mapped_type VehicleModule::GetCompositions(
			const ScheduledService& service
		){
			return _serviceCompositions[&service];
		}



		VehicleModule::LinesAllowedVehicles::mapped_type VehicleModule::GetAllowedVehicles(
			const pt::CommercialLine& line
		){
			return _linesAllowedVehicles[&line];
		}



		/// This method stores the current vehicle position in the database, only if the current vehicle is identified
		void VehicleModule::StoreCurrentVehiclePosition()
		{
			// Do not store the position if we do not know which vehicle is used
			if(!_currentVehiclePosition.getVehicle())
			{
				return;
			}

			VehiclePosition vpCopy(_currentVehiclePosition);
			vpCopy.setKey(VehiclePositionTableSync::getId());
			vpCopy.setTime(second_clock::local_time());
			VehiclePositionTableSync::Save(&vpCopy);
		}



		void VehicleModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
			if(name == MODULE_PARAM_CURRENT_VEHICLE_ID)
			{
				Vehicle* newCurrentVehicle(NULL);
				try
				{
					RegistryKeyType vehicleId(
						lexical_cast<RegistryKeyType>(value)
					);
					newCurrentVehicle = Env::GetOfficialEnv().getEditable<Vehicle>(vehicleId).get();
				}
				catch(bad_lexical_cast&)
				{
				}
				catch(ObjectNotFoundException<Vehicle>&)
				{
				}
				GetCurrentVehiclePosition().setVehicle(newCurrentVehicle);
				StoreCurrentVehiclePosition();
			}
		}
}	}
