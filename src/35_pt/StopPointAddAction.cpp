
//////////////////////////////////////////////////////////////////////////
/// StopPointAddAction class implementation.
/// @file StopPointAddAction.cpp
/// @author Hugues Romain
/// @date 2010
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
#include "StopPointAddAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "DBModule.h"

#include <geos/geom/Point.h>

using namespace std;
using namespace geos::geom;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;
	using namespace db;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopPointAddAction>::FACTORY_KEY("StopPointAddAction");
	}

	namespace pt
	{
		const string StopPointAddAction::PARAMETER_PLACE_ID = Action_PARAMETER_PREFIX + "pl";
		const string StopPointAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string StopPointAddAction::PARAMETER_OPERATOR_CODE = Action_PARAMETER_PREFIX + "oc";
		const string StopPointAddAction::PARAMETER_X = Action_PARAMETER_PREFIX + "x";
		const string StopPointAddAction::PARAMETER_Y = Action_PARAMETER_PREFIX + "y";
		const string StopPointAddAction::PARAMETER_LONGITUDE = Action_PARAMETER_PREFIX + "lon";
		const string StopPointAddAction::PARAMETER_LATITUDE = Action_PARAMETER_PREFIX + "lat";

		
		
		ParametersMap StopPointAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE_ID, _place->getKey());
			}
			map.insert(PARAMETER_OPERATOR_CODE, _operatorCode);
			if(_point.get() && !_point->isEmpty())
			{
				map.insert(PARAMETER_X, _point->getX());
				map.insert(PARAMETER_Y, _point->getY());
			}
			map.insert(PARAMETER_NAME, _name);
			return map;
		}
		
		
		
		void StopPointAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_place = StopAreaTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PLACE_ID), *_env);
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("No such connection place");
			}

			_name = map.getDefault<string>(PARAMETER_NAME);
			_operatorCode = map.getDefault<string>(PARAMETER_OPERATOR_CODE);
			if(map.getDefault<double>(PARAMETER_X, 0) && map.getDefault<double>(PARAMETER_Y, 0))
			{
				_point = CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
					map.get<double>(PARAMETER_X),
					map.get<double>(PARAMETER_Y)
				);
			}
			else if(map.getDefault<double>(PARAMETER_LONGITUDE, 0) && map.getDefault<double>(PARAMETER_LATITUDE, 0))
			{
				_point = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
					*DBModule::GetStorageCoordinatesSystem().createPoint(
						map.get<double>(PARAMETER_LONGITUDE),
						map.get<double>(PARAMETER_LATITUDE)
				)	);
			}
		}
		
		
		
		void StopPointAddAction::run(
			Request& request
		){
			StopPoint object;
			object.setHub(_place.get());
			object.setName(_name);
			object.setCodeBySource(_operatorCode);
			object.setGeometry(_point);

			StopPointTableSync::Save(&object);

//			::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(object.getKey());
		}
		
		
		
		bool StopPointAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
