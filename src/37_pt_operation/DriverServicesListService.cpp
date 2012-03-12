
/** DriverServicesListService class implementation.
	@file DriverServicesListService.cpp
	@author Hugues Romain
	@date 2011

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "DriverServicesListService.hpp"

#include "DriverService.hpp"
#include "MimeTypes.hpp"
#include "PTOperationModule.hpp"
#include "RequestException.h"
#include "Request.h"
#include "SchedulesBasedService.h"
#include "VehicleService.hpp"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;
	using namespace pt;
	using namespace graph;
	using namespace pt_operation;
	

	template<>
	const string FactorableTemplate<FunctionWithSite<false>, DriverServicesListService>::FACTORY_KEY = "DriverServicesList";

	namespace pt_operation
	{
		const string DriverServicesListService::PARAMETER_DATE = "date";
		const string DriverServicesListService::PARAMETER_PAGE_ID = "p";
		const string DriverServicesListService::PARAMETER_VEHICLE_SERVICE_ID = "vehicle_service";

		const string DriverServicesListService::TAG_SERVICE = "vehicleService";
		const string DriverServicesListService::TAG_SERVICES = "vehicleServices";



		DriverServicesListService::DriverServicesListService():
			_page(NULL),
			_vehicleService(NULL)
		{}



		ParametersMap DriverServicesListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page)
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			else if(!_mimeType.empty())
			{
				MimeType::SaveToParametersMap(_mimeType, map);
			}
			if(!_date.is_not_a_date())
			{
				map.insert(PARAMETER_DATE, _date);
			}
			if(_vehicleService)
			{
				map.insert(PARAMETER_VEHICLE_SERVICE_ID, _vehicleService->getKey());
			}
			return map;
		}

		void DriverServicesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Date
			if(map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_date = day_clock::local_day();
			}
			else
			{
				_date = from_simple_string(map.get<string>(PARAMETER_DATE));
			}

			_page = getPage(map.getDefault<string>(PARAMETER_PAGE_ID));
			if(!_page)
			{
				MimeType::LoadFromRecord(_mimeType, map);
			}

			RegistryKeyType vsId(map.getDefault<RegistryKeyType>(PARAMETER_VEHICLE_SERVICE_ID, 0));
			if(vsId)
			{
				try
				{
					_vehicleService = _env->get<VehicleService>(vsId).get();
				}
				catch(ObjectNotFoundException<VehicleService>&)
				{
					throw RequestException("No such vehicle service");
				}
			}
		}



		util::ParametersMap DriverServicesListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;
			size_t rank(0);
			typedef set<pair<time_duration, const DriverService*> > Result;
			Result r;
			BOOST_FOREACH(const DriverService::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<DriverService>())
			{
				const DriverService& service(*item.second);

				// Date filter
				if(!service.isActive(_date))
				{
					continue;
				}

				// Schedule
				time_duration resultTime(service.getServiceBeginning());

				// Vehicle service filter
				if(_vehicleService)
				{
					bool result(false);
					BOOST_FOREACH(const DriverService::Chunks::value_type& chunk, service.getChunks())
					{
						if(chunk.vehicleService == _vehicleService)
						{
							result = true;
							resultTime = chunk.elements.begin()->service->getDepartureSchedule(chunk.elements.begin()->startRank, false);
							break;
						}
					}
					if(!result)
					{
						continue;
					}
				}

				r.insert(make_pair(resultTime, &service));
			}

			BOOST_FOREACH(const Result::value_type& item, r)
			{
				shared_ptr<ParametersMap> servicePM(new ParametersMap);
				item.second->toParametersMap(*servicePM, true, _vehicleService);
				map.insert(TAG_SERVICE, servicePM);
			}

			if(_page)
			{
				if(map.hasSubMaps(TAG_SERVICE))
				{
					BOOST_FOREACH(const shared_ptr<ParametersMap>& servicePM, map.getSubMaps(TAG_SERVICE))
					{
						_page->display(stream, request, *servicePM);
			}	}	}
			else if(_mimeType == MimeTypes::XML)
			{
				map.outputXML(stream, TAG_SERVICES, true);
			}
			else if(_mimeType == MimeTypes::JSON)
			{
				map.outputJSON(stream, TAG_SERVICES);
			}

			return map;
		}



		bool DriverServicesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string DriverServicesListService::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : _mimeType;
		}
}	}
