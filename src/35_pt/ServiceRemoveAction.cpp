
//////////////////////////////////////////////////////////////////////////
/// ServiceRemoveAction class implementation.
/// @file ServiceRemoveAction.cpp
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
#include "ServiceRemoveAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "Fetcher.h"
#include "Service.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	using namespace graph;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceRemoveAction>::FACTORY_KEY("ServiceRemoveAction");
	}

	namespace pt
	{
		const string ServiceRemoveAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "id";
		
		
		
		ParametersMap ServiceRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			return map;
		}
		
		
		
		void ServiceRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = Fetcher<Service>::Fetch(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID), *_env);
			}
			catch(ObjectNotFoundException<Service>&)
			{
				throw ActionException("No such service");
			}
		}
		
		
		
		void ServiceRemoveAction::run(
			Request& request
		){
			Fetcher<Service>::FetchRemove(*_service);
//			::AddDeleteEntry(*_object, request.getUser().get());
		}
		
		
		
		bool ServiceRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}
	}
}
