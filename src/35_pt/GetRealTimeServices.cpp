
//////////////////////////////////////////////////////////////////////////
/// GetRealTimeServices class implementation.
/// @file GetRealTimeServices.cpp
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

#include "GetRealTimeServices.hpp"

#include "ActionException.h"
#include "CommercialLine.h"
#include "ModuleClass.h"
#include "ParametersMap.h"
#include "Request.h"
#include "Session.h"
#include "ScheduledService.h"
#include "TransportNetwork.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	template<>
	const string FactorableTemplate<Function, pt::GetRealTimeServices>::FACTORY_KEY = "GetRealTimeServices";

	namespace pt
	{


		server::FunctionAPI GetRealTimeServices::getAPI() const
		{
			FunctionAPI api(
				"35_pt",
				"Query SYNTHESE for all services having real time updates",
				""
			);
			return api;
		}
		
		ParametersMap GetRealTimeServices::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			return map;
		}

		void GetRealTimeServices::_setFromParametersMap(const ParametersMap& map)
		{
			setOutputFormatFromMap(map, "");
		}
		
		
		
		ParametersMap GetRealTimeServices::run(
			ostream &stream,
			const Request &request
		) const
		{
			ParametersMap map;

			BOOST_FOREACH(const ScheduledService::Registry::value_type& service,
						  Env::GetOfficialEnv().getRegistry<ScheduledService>())
			{
				boost::shared_ptr<ScheduledService> servicePtr(service.second);

				if(!servicePtr->hasRealTimeData())
					continue;

				SchedulesBasedService::Schedules departuresTH(
					servicePtr->getDepartureSchedules(true /* theorical */, false /* real time */)
				);
				SchedulesBasedService::Schedules arrivalsTH(
					servicePtr->getArrivalSchedules(true /* theorical */, false /* real time */)
				);

				SchedulesBasedService::Schedules departuresRT(
					servicePtr->getDepartureSchedules(false /* theorical */, true /* real time */)
				);
				SchedulesBasedService::Schedules arrivalsRT(
					servicePtr->getArrivalSchedules(false /* theorical */, true /* real time */)
				);

				boost::shared_ptr<ParametersMap> subMap(new ParametersMap);
				subMap->insert(
							"SERVICE_ID",
							servicePtr->getKey()
							);
				subMap->insert(
							"JOURNEYPATTERN_ID",
							servicePtr->getRoute()->getKey()
							);
				subMap->insert(
							"JOURNEYPATTERN_NAME",
							servicePtr->getRoute()->getName()
							);
				subMap->insert(
							"COMMERCIAL_LINE_ID",
							servicePtr->getRoute()->getCommercialLine()->getKey()
							);
				subMap->insert(
							"NETWORK_ID",
							servicePtr->getRoute()->getCommercialLine()->getNetwork()->getKey()
							);
				subMap->insert(
							"NETWORK_NAME",
							servicePtr->getRoute()->getCommercialLine()->getNetwork()->getName()
							);
				subMap->insert(
							"COMMERCIAL_LINE_NAME",
							servicePtr->getRoute()->getCommercialLine()->getShortName()
							);
				subMap->insert(
							"THEORICAL_START_TIME",
							departuresTH[0]
						);

				ostringstream osArrivalsShift;
				for(size_t i(0); i < arrivalsRT.size(); ++i)
				{
					boost::posix_time::time_duration delta(arrivalsRT[i] - arrivalsTH[i]);
					osArrivalsShift << delta.total_seconds();
					if(i < arrivalsRT.size() - 1)
						osArrivalsShift << ",";
				}
				subMap->insert("ARRIVALS_SHIFT_SECONDS", osArrivalsShift.str());

				ostringstream osDeparturesShift;
				for(size_t i(0); i < departuresRT.size(); ++i)
				{
					boost::posix_time::time_duration delta(departuresRT[i] - departuresTH[i]);
					osDeparturesShift << delta.total_seconds();
					if(i < departuresRT.size() - 1)
						osDeparturesShift << ",";
				}
				subMap->insert("DEPARTURES_SHIFT_SECONDS", osDeparturesShift.str());

				map.insert("SERVICES", subMap);
			}

			outputParametersMap(
				map,
				stream,
				"REAL_TIME_SERVICES",
				""
			);
			return map;
		}
		
		
		
		bool GetRealTimeServices::isAuthorized(
			const Session* session
		) const {
			return true;
		}

		std::string GetRealTimeServices::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat(MimeTypes::XML);
		}

}	}

