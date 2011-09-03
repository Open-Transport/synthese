
//////////////////////////////////////////////////////////////////////////
/// ServiceUpdateAction class implementation.
/// @file ServiceUpdateAction.cpp
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
#include "ServiceUpdateAction.h"
#include "TransportNetworkRight.h"
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
		template<> const string FactorableTemplate<Action, pt::ServiceUpdateAction>::FACTORY_KEY("ServiceUpdateAction");
	}

	namespace pt
	{
		const string ServiceUpdateAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "id";
		const string ServiceUpdateAction::PARAMETER_SERVICE_NUMBER = Action_PARAMETER_PREFIX + "sn";
		const string ServiceUpdateAction::PARAMETER_TEAM_NUMBER = Action_PARAMETER_PREFIX + "te";



		ParametersMap ServiceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_OBJECT_ID, _service->getKey());
				map.insert(PARAMETER_SERVICE_NUMBER, _serviceNumber);
				map.insert(PARAMETER_TEAM_NUMBER, _teamNumber);
			}
			return map;
		}



		void ServiceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = Fetcher<Service>::FetchEditable(map.get<RegistryKeyType>(PARAMETER_OBJECT_ID), *_env);
			}
			catch(ObjectNotFoundException<Service>&)
			{
				throw ActionException("No such service");
			}

			_serviceNumber = map.get<string>(PARAMETER_SERVICE_NUMBER);

			if(dynamic_cast<ScheduledService*>(_service.get()))
			{
				_teamNumber = map.get<string>(PARAMETER_TEAM_NUMBER);
			}
		}


// TODO Handle datasource links update
		void ServiceUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			_service->setServiceNumber(_serviceNumber);
			if(dynamic_cast<ScheduledService*>(_service.get()))
			{
				static_cast<ScheduledService*>(_service.get())->setTeam(_teamNumber);
			}

			Fetcher<Service>::FetchSave(*_service);

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool ServiceUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
