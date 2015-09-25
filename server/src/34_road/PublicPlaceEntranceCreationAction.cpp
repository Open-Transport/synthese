
//////////////////////////////////////////////////////////////////////////
/// PublicPlaceEntranceCreationAction class implementation.
/// @file PublicPlaceEntranceCreationAction.cpp
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

#include "PublicPlaceEntranceCreationAction.hpp"

#include "ActionException.h"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "PublicPlaceTableSync.h"
#include "PublicPlaceEntranceTableSync.hpp"
#include "Request.h"
#include "RoadChunkTableSync.h"

#include <geos/io/WKTReader.h>
#include <geos/geom/Point.h>

using namespace std;

using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace security;
	using namespace util;

	template<>
	const string FactorableTemplate<Action, road::PublicPlaceEntranceCreationAction>::FACTORY_KEY = "PublicPlaceEntranceCreation";

	namespace road
	{
		const string PublicPlaceEntranceCreationAction::PARAMETER_PUBLIC_PLACE_ID = Action_PARAMETER_PREFIX + "_public_place_id";
		const string PublicPlaceEntranceCreationAction::PARAMETER_MAX_DISTANCE = Action_PARAMETER_PREFIX + "_max_distance";



		ParametersMap PublicPlaceEntranceCreationAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_MAX_DISTANCE, _maxDistance);
			if(_place.get())
			{
				map.insert(PARAMETER_PUBLIC_PLACE_ID, _place->get<Key>());
			}
			return map;
		}



		void PublicPlaceEntranceCreationAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Public place
			try
			{
				_place = PublicPlaceTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_PUBLIC_PLACE_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<PublicPlace>&)
			{
				throw ActionException("No such public place");
			}

			// Point
			WKTReader reader(&CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory());
			_point.reset(
				dynamic_cast<Point*>(
					reader.read(
						map.get<string>(
							ObjectUpdateAction::GetInputName<PointGeometry>()
			)	)	)	);
			_point = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*_point);

			// Max distance
			_maxDistance = map.getDefault<double>(PARAMETER_MAX_DISTANCE, 100);
			if(_maxDistance < 0)
			{
				throw ActionException("Max distance must be positive");
			}
		}



		void PublicPlaceEntranceCreationAction::run(
			Request& request
		){
			// Entrance creation
			PublicPlaceEntrance entrance;
			entrance.set<PublicPlace>(*_place);

			// The projection
			RoadChunkTableSync::ProjectAddress(
				*_point,
				_maxDistance,
				entrance
			);

			// Saving
			PublicPlaceEntranceTableSync::Save(&entrance);

			// Object creation id
			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(entrance.get<Key>());
			}
		}



		bool PublicPlaceEntranceCreationAction::isAuthorized(
			const Session* session
		) const {
			return true;
			// return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		PublicPlaceEntranceCreationAction::PublicPlaceEntranceCreationAction():
			_maxDistance(100)
		{}
}	}

