
/** 37_pt_operation module class implementation.
	@file PTOperationModule.cpp
	@author RCSobility
	@date 2011
	@since 3.2.1

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

#include "PTOperationModule.hpp"
#include "Vehicle.hpp"
#include "Composition.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace pt_operation;
	using namespace util;
	using namespace pt;
	

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,PTOperationModule>::FACTORY_KEY("37_pt_operation");
	}

	namespace pt_operation
	{
		PTOperationModule::LinesAllowedVehicles PTOperationModule::_linesAllowedVehicles;
		PTOperationModule::ServiceCompositions PTOperationModule::_serviceCompositions;
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<PTOperationModule>::NAME("Exploitation transport public");

		template<> void ModuleClassTemplate<PTOperationModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<PTOperationModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<PTOperationModule>::End()
		{
		}
	}

	namespace pt_operation
	{
		void PTOperationModule::RegisterVehicle(const Vehicle& vehicle )
		{
			BOOST_FOREACH(const Vehicle::AllowedLines::value_type& line, vehicle.getAllowedLines())
			{
				_linesAllowedVehicles[line].insert(&vehicle);
			}
		}



		void PTOperationModule::UnregisterVehicle(const Vehicle& vehicle )
		{
			BOOST_FOREACH(const Vehicle::AllowedLines::value_type& line, vehicle.getAllowedLines())
			{
				_linesAllowedVehicles[line].erase(&vehicle);
			}
		}



		void PTOperationModule::RegisterComposition(const Composition& composition )
		{
			_serviceCompositions[composition.getService()].insert(&composition);
		}



		void PTOperationModule::UnregisterComposition(const Composition& composition )
		{
			_serviceCompositions[composition.getService()].erase(&composition);
		}



		PTOperationModule::ServiceCompositions::mapped_type PTOperationModule::GetCompositions(const ScheduledService& service )
		{
			return _serviceCompositions[&service];
		}



		PTOperationModule::LinesAllowedVehicles::mapped_type PTOperationModule::GetAllowedVehicles(const pt::CommercialLine& line )
		{
			return _linesAllowedVehicles[&line];
		}
}	}
