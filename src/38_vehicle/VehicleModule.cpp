
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

#include "ServiceComposition.hpp"
#include "Vehicle.hpp"

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
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<VehicleModule>::NAME = "Vehicule";

		template<> void ModuleClassTemplate<VehicleModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<VehicleModule>::Init()
		{
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
			BOOST_FOREACH(const Vehicle::AllowedLines::value_type& line, vehicle.getAllowedLines())
			{
				_linesAllowedVehicles[line].insert(&vehicle);
			}
		}



		void VehicleModule::UnregisterVehicle(const Vehicle& vehicle )
		{
			BOOST_FOREACH(const Vehicle::AllowedLines::value_type& line, vehicle.getAllowedLines())
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
}	}
