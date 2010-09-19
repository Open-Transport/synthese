
//////////////////////////////////////////////////////////////////////////
/// StopPointMoveAction class implementation.
/// @file StopPointMoveAction.cpp
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
#include "StopPointMoveAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopPointTableSync.hpp"

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
		template<> const string FactorableTemplate<Action, pt::StopPointMoveAction>::FACTORY_KEY("StopPointMoveAction");
	}

	namespace pt
	{
		const string StopPointMoveAction::PARAMETER_STOP_ID = Action_PARAMETER_PREFIX + "id";
		const string StopPointMoveAction::PARAMETER_LONGITUDE = Action_PARAMETER_PREFIX + "lon";
		const string StopPointMoveAction::PARAMETER_LATITUDE = Action_PARAMETER_PREFIX + "lat";
		
		
		
		ParametersMap StopPointMoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_STOP_ID, _stop->getKey());
			}
			return map;
		}
		
		
		
		void StopPointMoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_stop = StopPointTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_STOP_ID), *_env);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such physical stop");
			}
			_point = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
				*DBModule::GetStorageCoordinatesSystem().createPoint(
					map.get<double>(PARAMETER_LONGITUDE),
					map.get<double>(PARAMETER_LATITUDE)
			)	);
		}
		
		
		
		void StopPointMoveAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			_stop->setGeometry(_point);

			StopPointTableSync::Save(_stop.get());
//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool StopPointMoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
