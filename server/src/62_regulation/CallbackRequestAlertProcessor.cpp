//////////////////////////////////////////////////////////////////////////
/// CallbackRequestAlertProcessor class implementation.
///	@file CallbackRequestAlertProcessor.cpp
///	@author Marc Jambert
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
#include "CallbackRequestAlertProcessor.hpp"

#include "AlertTableSync.hpp"
#include "VehicleCall.hpp"
#include "VehicleCallTableSync.hpp"
#include "VehiclePositionTableSync.hpp"
#include "DB.hpp"
#include "DBModule.h"
#include "Env.h"
#include "ScheduledService.h"
#include "ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace vehicle;
    
	namespace regulation
	{

		CallbackRequestAlertProcessor::CallbackRequestAlertProcessor()
			: AlertProcessor()
		{
		}

        
		void
		CallbackRequestAlertProcessor::processAlerts()
		{
			AlertTableSync::SearchResult callbackRequestAlerts(AlertTableSync::Search(Env::GetOfficialEnv(), ALERT_TYPE_CALLBACKREQUEST));

			std::map<util::RegistryKeyType, boost::shared_ptr<VehicleCall> > latestVehicleCalls;
			BOOST_FOREACH(const VehicleCall::Registry::value_type& itVehicleCall,
				Env::GetOfficialEnv().getRegistry<VehicleCall>())
			{
				boost::shared_ptr<VehicleCall> vehicleCall = itVehicleCall.second;
				boost::optional<Vehicle&> vehicle = vehicleCall->get<Vehicle>();

				if (!vehicle) continue;

				std::map<util::RegistryKeyType, boost::shared_ptr<VehicleCall> >::const_iterator latestVehicleCallIt(
				latestVehicleCalls.find(vehicle->getKey()));
				if ((latestVehicleCallIt == latestVehicleCalls.end()) ||
				(vehicleCall->get<CallTime>() > latestVehicleCallIt->second->get<CallTime>()))
				{
					latestVehicleCalls[vehicle->getKey()] = vehicleCall;
				}
            }

				for (std::map<util::RegistryKeyType, boost::shared_ptr<VehicleCall> >::iterator itVehicleCall = latestVehicleCalls.begin();
					itVehicleCall != latestVehicleCalls.end();
					++itVehicleCall)
			{
				boost::shared_ptr<VehicleCall> vehicleCall = itVehicleCall->second;
				boost::optional<Vehicle&> vehicle = vehicleCall->get<Vehicle>();

				VehiclePositionTableSync::SearchResult vehiclePositions(
					VehiclePositionTableSync::Search(Env::GetOfficialEnv(), vehicle->getKey()));

				if (vehiclePositions.empty()) continue;
				boost::shared_ptr<const VehiclePosition> vehiclePosition = *vehiclePositions.begin();

				pt::ScheduledService* scheduledService = vehiclePosition->getService();
				if(NULL == scheduledService) continue;

				const pt::JourneyPattern* journeyPattern(scheduledService->getRoute());
				if(NULL == journeyPattern) continue;

				pt::CommercialLine* commercialLine(journeyPattern->getCommercialLine());
				if(NULL == commercialLine) continue;

				util::RegistryKeyType commercialLineId(commercialLine->getKey());
				bool wasExisting(false);
				BOOST_FOREACH(const boost::shared_ptr<Alert>& callbackRequestAlert, callbackRequestAlerts)
				{
					if (commercialLineId != callbackRequestAlert->get<Line>().get().getKey()) continue;
					if (scheduledService->getKey() != callbackRequestAlert->get<Service>().get().getKey()) continue;

					wasExisting = true;

					// check if need update
					util::ParametersMap extraDataPM;
					extraDataPM.insert("priority", vehicleCall->get<Priority>());
					std::stringstream extraDataStream;
					extraDataPM.outputJSON(extraDataStream, "extraData");
					if (extraDataStream.str() != callbackRequestAlert->get<ExtraData>())
					{
						//std::cerr << " old JSON : " << callbackRequestAlert->get<ExtraData>() << std::endl;
						//std::cerr << " new JSON : " << extraDataStream.str() << std::endl;
						//std::cerr << " Need update !!!!" << std::endl;
						callbackRequestAlert->set<ExtraData>(extraDataStream.str());
						AlertTableSync::Save(callbackRequestAlert.get());
					}
					break;
				}

				if (!wasExisting)
				{
					Alert callbackRequestAlert;
					callbackRequestAlert.set<Kind>(ALERT_TYPE_CALLBACKREQUEST);
					callbackRequestAlert.set<Service>(*scheduledService);
					callbackRequestAlert.set<Line>(*commercialLine);

					util::ParametersMap extraDataPM;
					extraDataPM.insert("priority", vehicleCall->get<Priority>());
					std::stringstream extraDataStream;
					extraDataPM.outputJSON(extraDataStream, "extraData");
					callbackRequestAlert.set<ExtraData>(extraDataStream.str());

					AlertTableSync::Save(&callbackRequestAlert);
				}
			}

		}

	}
}
