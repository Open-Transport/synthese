
//////////////////////////////////////////////////////////////////////////
/// CompositionVehicleRemoveAction class implementation.
/// @file CompositionVehicleRemoveAction.cpp
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
#include "CompositionVehicleRemoveAction.hpp"
#include "Request.h"
#include "Composition.hpp"
#include "CompositionTableSync.hpp"
#include "Vehicle.hpp"
#include "VehicleTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vehicle::CompositionVehicleRemoveAction>::FACTORY_KEY("CompositionVehicleRemoveAction");
	}

	namespace vehicle
	{
		const string CompositionVehicleRemoveAction::PARAMETER_COMPOSITION_ID = Action_PARAMETER_PREFIX + "ci";
		const string CompositionVehicleRemoveAction::PARAMETER_VEHICLE_ID = Action_PARAMETER_PREFIX + "vi";



		ParametersMap CompositionVehicleRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_composition.get())
			{
				map.insert(PARAMETER_COMPOSITION_ID, _composition->getKey());
			}
			if(_vehicle.get())
			{
				map.insert(PARAMETER_VEHICLE_ID, _vehicle->getKey());
			}
			return map;
		}



		void CompositionVehicleRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_composition = CompositionTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_COMPOSITION_ID), *_env);
			}
			catch(ObjectNotFoundException<Composition>&)
			{
				throw ActionException("No such composition");
			}

			try
			{
				_vehicle = VehicleTableSync::Get(map.get<RegistryKeyType>(PARAMETER_VEHICLE_ID), *_env);
			}
			catch(ObjectNotFoundException<Vehicle>&)
			{
				throw ActionException("No such vehicle");
			}
		}



		void CompositionVehicleRemoveAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			Composition::VehicleLinks links(_composition->getVehicles());
			for(Composition::VehicleLinks::iterator it(links.begin()); it != links.end(); ++it)
			{
				if(it->vehicle == _vehicle.get())
				{
					links.erase(it);
					break;
				}
			}
			_composition->setVehicles(links);

			CompositionTableSync::Save(_composition.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CompositionVehicleRemoveAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
