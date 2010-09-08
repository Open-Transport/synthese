
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternUpdateAction class implementation.
/// @file JourneyPatternUpdateAction.cpp
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
#include "JourneyPatternUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "JourneyPatternTableSync.hpp"
#include "RollingStockTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::JourneyPatternUpdateAction>::FACTORY_KEY("JourneyPatternUpdateAction");
	}

	namespace pt
	{
		const string JourneyPatternUpdateAction::PARAMETER_ROUTE_ID = Action_PARAMETER_PREFIX + "id";
		const string JourneyPatternUpdateAction::PARAMETER_TRANSPORT_MODE_ID = Action_PARAMETER_PREFIX + "tm";
		const string JourneyPatternUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string JourneyPatternUpdateAction::PARAMETER_DIRECTION = Action_PARAMETER_PREFIX + "di";
		const string JourneyPatternUpdateAction::PARAMETER_WAYBACK = Action_PARAMETER_PREFIX + "wb";
		
		
		
		ParametersMap JourneyPatternUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_route.get())
			{
				map.insert(PARAMETER_ROUTE_ID, _route->getKey());
				map.insert(PARAMETER_NAME, _name);
				map.insert(PARAMETER_DIRECTION, _direction);
				map.insert(PARAMETER_WAYBACK, _wayback);
				map.insert(PARAMETER_TRANSPORT_MODE_ID, _transportMode.get() ? _transportMode->getKey() : RegistryKeyType(0));
			}
			return map;
		}
		
		
		
		void JourneyPatternUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_route = JourneyPatternTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROUTE_ID), *_env);
			}
			catch(ObjectNotFoundException<JourneyPattern>&)
			{
				throw ActionException("No such route");
			}
			_direction = map.get<string>(PARAMETER_DIRECTION);
			_name = map.get<string>(PARAMETER_NAME);
			_wayback = map.get<bool>(PARAMETER_WAYBACK);

			optional<RegistryKeyType> rid(map.getOptional<RegistryKeyType>(PARAMETER_TRANSPORT_MODE_ID));
			if(rid && *rid > 0)	try
			{
				_transportMode = RollingStockTableSync::GetEditable(*rid, *_env);
			}
			catch(ObjectNotFoundException<RollingStock>&)
			{
				throw ActionException("No such transport mode");
			}
		}
		
		
		
		void JourneyPatternUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			_route->setName(_name);
			_route->setRollingStock(_transportMode.get());
			_route->setDirection(_direction);
			_route->setWayBack(_wayback);
			
			JourneyPatternTableSync::Save(_route.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool JourneyPatternUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
