
//////////////////////////////////////////////////////////////////////////
/// PhysicalStopMoveAction class implementation.
/// @file PhysicalStopMoveAction.cpp
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
#include "PhysicalStopMoveAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "Projection.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;
	using namespace geometry;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::PhysicalStopMoveAction>::FACTORY_KEY("PhysicalStopMoveAction");
	}

	namespace pt
	{
		const string PhysicalStopMoveAction::PARAMETER_STOP_ID = Action_PARAMETER_PREFIX + "id";
		const string PhysicalStopMoveAction::PARAMETER_LONGITUDE = Action_PARAMETER_PREFIX + "lon";
		const string PhysicalStopMoveAction::PARAMETER_LATITUDE = Action_PARAMETER_PREFIX + "lat";
		
		
		
		ParametersMap PhysicalStopMoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_STOP_ID, _stop->getKey());
			}
			return map;
		}
		
		
		
		void PhysicalStopMoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_stop = StopPointTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_STOP_ID), *_env);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such physical stop");
			}
			_point.setLongitude(map.get<double>(PARAMETER_LONGITUDE));
			_point.setLatitude(map.get<double>(PARAMETER_LATITUDE));
		}
		
		
		
		void PhysicalStopMoveAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			Point2D projected(LambertFromWGS84(_point));
			_stop->setXY(projected.getX(), projected.getY());

			StopPointTableSync::Save(_stop.get());
//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool PhysicalStopMoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
