
//////////////////////////////////////////////////////////////////////////
/// ServiceUpdateAction class implementation.
/// @file ServiceUpdateAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "ServiceUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "Fetcher.h"
#include "ScheduledService.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceUpdateAction>::FACTORY_KEY("service_update");
	}

	namespace pt
	{
		const string ServiceUpdateAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "id";
		const string ServiceUpdateAction::PARAMETER_SERVICE_NUMBER = Action_PARAMETER_PREFIX + "sn";
		const string ServiceUpdateAction::PARAMETER_TEAM_NUMBER = Action_PARAMETER_PREFIX + "te";



		ParametersMap ServiceUpdateAction::getParametersMap() const
		{
			ParametersMap map;

			// The service
			if(_service.get())
			{
				map.insert(PARAMETER_OBJECT_ID, _service->getKey());
			}

			// Service number
			if(_serviceNumber)
			{
				map.insert(PARAMETER_SERVICE_NUMBER, *_serviceNumber);
			}

			// Team number
			if(_teamNumber)
			{
				map.insert(PARAMETER_TEAM_NUMBER, *_teamNumber);
			}

			// Date by value
			_getCalendarUpdateParametersMap(map);

			return map;
		}



		void ServiceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// The service
			try
			{
				_service = Fetcher<SchedulesBasedService>::FetchEditable(
					map.get<RegistryKeyType>(PARAMETER_OBJECT_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<SchedulesBasedService>&)
			{
				throw ActionException("No such service");
			}

			// Service number
			if(map.isDefined(PARAMETER_SERVICE_NUMBER))
			{
				_serviceNumber = map.get<string>(PARAMETER_SERVICE_NUMBER);
			}

			// Team number
			if(	map.isDefined(PARAMETER_TEAM_NUMBER) &&
				dynamic_cast<ScheduledService*>(_service.get())
			){
				_teamNumber = map.get<string>(PARAMETER_TEAM_NUMBER);
			}

			// Calendar by date
			_setCalendarUpdateFromParametersMap(*_env, map);
		}


// TODO Handle datasource links update
		void ServiceUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			// Service number
			if(_serviceNumber)
			{
				_service->setServiceNumber(*_serviceNumber);
			}

			// Team number
			if(_teamNumber)
			{
				static_cast<ScheduledService*>(_service.get())->setTeam(*_teamNumber);
			}

			// Calendar by date
			_doCalendarUpdate(*_service, request);

			// Save
			Fetcher<SchedulesBasedService>::FetchSave(*_service);

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool ServiceUpdateAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this service
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}
}	}
