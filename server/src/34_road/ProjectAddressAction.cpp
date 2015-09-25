
//////////////////////////////////////////////////////////////////////////
/// ProjectAddressAction class implementation.
/// @file ProjectAddressAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "ProjectAddressAction.hpp"

#include "ActionException.h"
#include "Address.h"
#include "GeometryField.hpp"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "RoadChunkTableSync.h"

#include <geos/io/WKTReader.h>

using namespace boost;
using namespace std;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace db;
	using namespace geography;
	using namespace server;
	using namespace security;
	using namespace util;

	template<>
	const string FactorableTemplate<Action, road::ProjectAddressAction>::FACTORY_KEY = "ProjectAddress";

	namespace road
	{
		const string ProjectAddressAction::PARAMETER_ADDRESS_ID = Action_PARAMETER_PREFIX + "_addess_id";
		const string ProjectAddressAction::PARAMETER_MAX_DISTANCE = Action_PARAMETER_PREFIX + "_max_distance";



		ParametersMap ProjectAddressAction::getParametersMap() const
		{
			ParametersMap map;

			// Address
			if(_address.get())
			{
				map.insert(PARAMETER_ADDRESS_ID, _address->getKey());
			}

			// Max distance
			map.insert(PARAMETER_MAX_DISTANCE, _maxDistance);

			return map;
		}



		void ProjectAddressAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Address
			try
			{
				_address = DBModule::GetEditableObject(
					map.get<RegistryKeyType>(PARAMETER_ADDRESS_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<util::Registrable>&)
			{
				throw ActionException("No such address");
			}

			if(!dynamic_cast<Address*>(_address.get()))
			{
				throw ActionException("The object to update is not an address");
			}

			// Point
			WKTReader reader(&CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory());
			_point.reset(
				dynamic_cast<Point*>(
					reader.read(
						map.get<string>(
							ObjectUpdateAction::GetInputName<PointGeometry>()
			)	)	)	);

			// Max distance
			_maxDistance = map.getDefault<double>(PARAMETER_MAX_DISTANCE, 100);
			if(_maxDistance < 0)
			{
				throw ActionException("Max distance must be positive");
			}
		}



		void ProjectAddressAction::run(
			Request& request
		){
			// Conversion of the address
			Address& address(dynamic_cast<Address&>(*_address));

			// The projection
			RoadChunkTableSync::ProjectAddress(
				*_point,
				_maxDistance,
				address
			);

			// Saving
			DBModule::SaveObject(*_address);
		}



		bool ProjectAddressAction::isAuthorized(
			const Session* session
		) const {
			return true;
			// return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		ProjectAddressAction::ProjectAddressAction():
			_maxDistance(100)
		{}
}	}

