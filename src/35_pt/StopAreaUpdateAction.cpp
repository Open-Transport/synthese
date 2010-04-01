
//////////////////////////////////////////////////////////////////////////
/// StopAreaUpdateAction class implementation.
/// @file StopAreaUpdateAction.cpp
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
#include "StopAreaUpdateAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"

using namespace std;
using namespace boost::posix_time;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace env;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopAreaUpdateAction>::FACTORY_KEY("StopAreaUpdateAction");
	}

	namespace pt
	{
		const string StopAreaUpdateAction::PARAMETER_ALLOWED_CONNECTIONS = Action_PARAMETER_PREFIX + "ac";
		const string StopAreaUpdateAction::PARAMETER_PLACE_ID = Action_PARAMETER_PREFIX + "id";
		const string StopAreaUpdateAction::PARAMETER_DEFAULT_TRANSFER_DURATION = Action_PARAMETER_PREFIX + "dt";

		
		
		ParametersMap StopAreaUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE_ID, _place->getKey());
				map.insert(PARAMETER_ALLOWED_CONNECTIONS, _allowedConnections);
				map.insert(PARAMETER_DEFAULT_TRANSFER_DURATION, _defaultTransferDuration.total_seconds() / 60);
			}
			return map;
		}
		
		
		
		void StopAreaUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_place = ConnectionPlaceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PLACE_ID), *_env);
			}
			catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>&)
			{
				throw ActionException("No such place");
			}

			_allowedConnections = map.get<bool>(PARAMETER_ALLOWED_CONNECTIONS);
			_defaultTransferDuration = minutes(map.get<int>(PARAMETER_DEFAULT_TRANSFER_DURATION));
		}
		
		
		
		void StopAreaUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			_place->setAllowedConnection(_allowedConnections);
			_place->setDefaultTransferDelay(_defaultTransferDuration);

			ConnectionPlaceTableSync::Save(_place.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool StopAreaUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
