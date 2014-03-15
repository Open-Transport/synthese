
//////////////////////////////////////////////////////////////////////////////////////////
///	VehicleServiceUsagesListService class implementation.
///	@file VehicleServiceUsagesListService.cpp
///	@author Hugues Romain
///	@date 2014
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

#include "VehicleServiceUsagesListService.hpp"

#include "ActionException.h"
#include "VehicleServiceTableSync.hpp"
#include "VehicleServiceUsageTableSync.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace pt_operation;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,analysis::VehicleServiceUsagesListService>::FACTORY_KEY = "vehicle_service_usages";
	
	namespace analysis
	{
		const string VehicleServiceUsagesListService::PARAMETER_START_DATE = "start_date";
		const string VehicleServiceUsagesListService::PARAMETER_END_DATE = "end_date";
		const string VehicleServiceUsagesListService::PARAMETER_WITH_DETAIL = "with_detail";

		const string VehicleServiceUsagesListService::TAG_VEHICLE_SERVICE = "vehicle_service";
		const string VehicleServiceUsagesListService::TAG_USAGE = "usage";
		


		ParametersMap VehicleServiceUsagesListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VehicleServiceUsagesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Dates
			_startDate = from_simple_string(map.get<string>(PARAMETER_START_DATE));
			_endDate = from_simple_string(map.get<string>(PARAMETER_END_DATE));

			if(_startDate.is_not_a_date() || _endDate.is_not_a_date())
			{
				throw ActionException("Start and end dates are not defined");
			}

			if(_startDate < _endDate)
			{
				date swap(_startDate);
				_startDate = _endDate;
				_endDate = swap;
			}

			// Filter
			RegistryKeyType id(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			if(decodeTableId(id) == OperationUnit::CLASS_NUMBER)
			{
				_services = VehicleServiceTableSync::Search(
					Env::GetOfficialEnv(),
					optional<string>(),
					id
				);
			}
			else if(decodeTableId(id) == VehicleService::CLASS_NUMBER)
			{
				_services.push_back(Env::GetOfficialEnv().getEditable<VehicleService>(id));
			}
			else
			{
				BOOST_FOREACH(const VehicleService::Registry::value_type it, Env::GetOfficialEnv().getRegistry<VehicleService>())
				{
					_services.push_back(it.second);
				}
			}

			// With detail
			_withDetail = map.getDefault<bool>(PARAMETER_WITH_DETAIL, true);

		}



		ParametersMap VehicleServiceUsagesListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;
			Env usageEnv;

			VehicleServiceUsage sumUsage;

			BOOST_FOREACH(const boost::shared_ptr<VehicleService>& itServ, _services)
			{
				boost::shared_ptr<ParametersMap> servicePM(new ParametersMap);
				VehicleServiceUsage serviceSumUsage;

				itServ->toParametersMap(*servicePM, false);

				for(date itDate(_startDate); itDate <= _endDate; itDate += days(1))
				{
					VehicleServiceUsageTableSync::SearchResult usages(
						VehicleServiceUsageTableSync::Search(
							usageEnv,
							itDate,
							*itServ
					)	);

					BOOST_FOREACH(boost::shared_ptr<VehicleServiceUsage> usage, usages)
					{
						serviceSumUsage += *usage;
						sumUsage += *usage;

						if(_withDetail)
						{
							boost::shared_ptr<ParametersMap> dayPM(new ParametersMap);
							usage->toParametersMap(*dayPM, true);
							map.insert(TAG_USAGE, dayPM);
						}
					}
				}

				serviceSumUsage.toParametersMap(*servicePM, true);

				map.insert(TAG_VEHICLE_SERVICE, servicePM);
			}

			sumUsage.toParametersMap(map, true);

			return map;
		}
		
		
		
		bool VehicleServiceUsagesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VehicleServiceUsagesListService::getOutputMimeType() const
		{
			return "text/html";
		}



		VehicleServiceUsagesListService::VehicleServiceUsagesListService():
			_withDetail(true)
		{

		}
}	}
