
//////////////////////////////////////////////////////////////////////////
/// ServiceQuotaUpdateAction class implementation.
/// @file ServiceQuotaUpdateAction.cpp
/// @author Gael Sauvanet
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

#include "ServiceQuotaUpdateAction.hpp"

#include "ActionException.h"
#include "RequestException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ServiceQuota.hpp"
#include "ServiceQuotaTableSync.hpp"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceQuotaUpdateAction>::FACTORY_KEY("ServiceQuotaUpdate");
	}

	namespace pt
	{
		const string ServiceQuotaUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "service_id";
		const string ServiceQuotaUpdateAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "date";
		const string ServiceQuotaUpdateAction::PARAMETER_QUOTA = Action_PARAMETER_PREFIX + "quota";


		ParametersMap ServiceQuotaUpdateAction::getParametersMap() const
		{
			ParametersMap map;

			// Service
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}

			return map;
		}



		void ServiceQuotaUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Service
			try
			{
				_service = ScheduledServiceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID), *_env);
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
				throw RequestException("No such service");
			}
			
			// ServiceQuotas
			ServiceQuotaTableSync::SearchResult serviceQuotas(
				ServiceQuotaTableSync::Search(*_env,_service->getKey())
				);
			// Try to get a ServiceQuota for this Service
			if(serviceQuotas.size() == 1)
			{
				_serviceQuota = serviceQuotas[0];
			}
			else
			{
				_serviceQuota.reset(new ServiceQuota);
				_serviceQuota->set<Service>(*_service);
			}

			// Date
			if(map.getDefault<string>(PARAMETER_DATE).empty())
			{
				throw RequestException("No date parameter");
			}
			else
			{
				_date = from_simple_string(map.get<string>(PARAMETER_DATE));
			}

			// Quota
			_quota = map.getDefault<int>(PARAMETER_QUOTA, 0);
		}



		void ServiceQuotaUpdateAction::run(
			Request& request
		){
			_serviceQuota->get<Quotas>()[_date] = _quota;

			ServiceQuotaTableSync::Save(_serviceQuota.get());
		}



		bool ServiceQuotaUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}