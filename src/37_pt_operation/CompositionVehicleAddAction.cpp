
//////////////////////////////////////////////////////////////////////////
/// CompositionVehicleAddAction class implementation.
/// @file CompositionVehicleAddAction.cpp
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
#include "CompositionVehicleAddAction.hpp"
#include "Request.h"
#include "CompositionTableSync.hpp"
#include "VehicleTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_operation::CompositionVehicleAddAction>::FACTORY_KEY("CompositionVehicleAddAction");
	}

	namespace pt_operation
	{
		const string CompositionVehicleAddAction::PARAMETER_COMPOSITION_ID = Action_PARAMETER_PREFIX + "ci";
		const string CompositionVehicleAddAction::PARAMETER_VEHICLE_ID = Action_PARAMETER_PREFIX + "vi";
		const string CompositionVehicleAddAction::PARAMETER_NUMBER = Action_PARAMETER_PREFIX + "nu";



		ParametersMap CompositionVehicleAddAction::getParametersMap() const
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
			map.insert(PARAMETER_NUMBER, _number);
			return map;
		}



		void CompositionVehicleAddAction::_setFromParametersMap(const ParametersMap& map)
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
				_vehicle = VehicleTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_VEHICLE_ID), *_env);
			}
			catch(ObjectNotFoundException<Vehicle>&)
			{
				throw ActionException("No such vehicle");
			}

			_number = map.getDefault<string>(PARAMETER_NUMBER);
		}



		void CompositionVehicleAddAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			Composition::VehicleLink link;
			link.number = _number;
			link.vehicle = _vehicle.get();
			Composition::VehicleLinks links(_composition->getVehicles());
			links.push_back(link);
			_composition->setVehicles(links);

			CompositionTableSync::Save(_composition.get());
			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CompositionVehicleAddAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
