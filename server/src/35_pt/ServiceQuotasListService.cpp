
//////////////////////////////////////////////////////////////////////////////////////////
/// ServiceQuotasListService class implementation.
///	@file ServiceQuotasListService.cpp
///	@author Gael Sauvanet
///	@date 2012
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

#include "ServiceQuotasListService.hpp"

#include "ServiceQuotaTableSync.hpp"
#include "ServiceQuota.hpp"
#include "MimeTypes.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace algorithm;
using namespace boost::gregorian;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace server;
	using namespace pt;

	template<>
	const string FactorableTemplate<FunctionWithSite<false>, ServiceQuotasListService>::FACTORY_KEY = "service_quotas";

	namespace pt
	{
		const string ServiceQuotasListService::PARAMETER_SERVICE_ID = "service_id";
		const string ServiceQuotasListService::PARAMETER_MIN_DATE = "min_date";
		const string ServiceQuotasListService::PARAMETER_MAX_DATE = "max_date";
		const string ServiceQuotasListService::PARAMETER_PAGE_ID = "p";

		const string ServiceQuotasListService::TAG_SERVICE_QUOTA = "service_quota";
		const string ServiceQuotasListService::TAG_SERVICE_QUOTAS = "service_quotas";


		ServiceQuotasListService::ServiceQuotasListService():
			_minDate(not_a_date_time),
			_maxDate(not_a_date_time)
		{}



		ParametersMap ServiceQuotasListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page)
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			if(_service)
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			return map;
		}



		void ServiceQuotasListService::_setFromParametersMap(const ParametersMap& map)
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

			// Min Date
			if(map.getDefault<string>(PARAMETER_MIN_DATE).empty())
			{
				_minDate = day_clock::local_day();
			}
			else
			{
				_minDate = from_simple_string(map.get<string>(PARAMETER_MIN_DATE));
			}

			// Max Date
			if(map.getDefault<string>(PARAMETER_MAX_DATE).empty())
			{
				_maxDate = day_clock::local_day();
			}
			else
			{
				_maxDate = from_simple_string(map.get<string>(PARAMETER_MAX_DATE));
			}

			// Display page
			optional<RegistryKeyType> pid(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(pid) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(*pid);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such main page");
			}
			if(!_page.get())
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
			}
		}



		util::ParametersMap ServiceQuotasListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap result;

			// ServiceQuotas
			boost::shared_ptr<ServiceQuota> serviceQuota;
			ServiceQuotaTableSync::SearchResult serviceQuotas(
				ServiceQuotaTableSync::Search(*_env,_service->getKey())
				);
			// Try to get ServiceQuota for this Service
			if(serviceQuotas.size() == 1)
			{
				boost::shared_ptr<ParametersMap> serviceQuotaPM(new ParametersMap);

				serviceQuota = serviceQuotas[0];

				QuotasMap::iterator start_iter = serviceQuota->get<Quotas>().lower_bound(_minDate);
				QuotasMap::iterator end_iter = serviceQuota->get<Quotas>().upper_bound(_maxDate);

				serviceQuotaPM->insert(PARAMETER_SERVICE_ID, serviceQuota->get<Service>()->getKey());

				for (QuotasMap::iterator quota = start_iter; quota != end_iter; ++quota)
				{
					boost::shared_ptr<ParametersMap> quotaPM(new ParametersMap);
					quotaPM->insert("date", (*quota).first);
					quotaPM->insert("n", (*quota).second);
					serviceQuotaPM->insert("quota", quotaPM);
				}
				
				result.insert(TAG_SERVICE_QUOTA, serviceQuotaPM);
			}

			// CMS Display
			if(_page)
			{
				_page->display(stream, request, result);
			}
			else
			{
				outputParametersMap(
					result,
					stream,
					TAG_SERVICE_QUOTAS,
					""
				);
			}
			return result;
		}



		bool ServiceQuotasListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ServiceQuotasListService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}
}	}
