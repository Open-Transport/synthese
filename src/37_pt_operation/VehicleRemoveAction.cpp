
//////////////////////////////////////////////////////////////////////////
/// VehicleRemoveAction class implementation.
/// @file VehicleRemoveAction.cpp
/// @author RCSobility
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ActionException.h"
#include "ParametersMap.h"
#include "VehicleRemoveAction.hpp"
#include "Request.h"
#include "Vehicle.hpp"
#include "VehicleTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_operation::VehicleRemoveAction>::FACTORY_KEY("VehicleRemoveAction");
	}

	namespace pt_operation
	{
		const string VehicleRemoveAction::PARAMETER_VEHICLE_ID = Action_PARAMETER_PREFIX + "vi";
		
		
		
		ParametersMap VehicleRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_vehicle.get())
			{
				map.insert(PARAMETER_VEHICLE_ID, _vehicle->getKey());
			}
			return map;
		}
		
		
		
		void VehicleRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_vehicle = VehicleTableSync::Get(map.get<RegistryKeyType>(PARAMETER_VEHICLE_ID), *_env);
			}
			catch(ObjectNotFoundException<Vehicle>&)
			{
				throw ActionException("No such vehicle");
			}
		}
		
		
		
		void VehicleRemoveAction::run(
			Request& request
		){
			VehicleTableSync::Remove(_vehicle->getKey());
			//::AddDeleteEntry(*_object, *request.getUser());
		}
		
		
		
		bool VehicleRemoveAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
