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
#include "SelectQuery.hpp"
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
            util::Log::GetInstance().debug("Processing callback requests alerts");

			BOOST_FOREACH(const VehicleCall::Registry::value_type& itVehicleCall, //registry)
                          Env::GetOfficialEnv().getRegistry<VehicleCall>())
            {

                boost::shared_ptr<VehicleCall> vehicleCall = itVehicleCall.second;
                boost::optional<Vehicle&> vehicle = vehicleCall->get<Vehicle>();

                // TODO check exists
                if (!vehicle) continue;
                
                VehiclePositionTableSync::SearchResult vehiclePositions(
                    VehiclePositionTableSync::Search(Env::GetOfficialEnv(), vehicle->getKey()));

                // TODO check size
                boost::shared_ptr<const VehiclePosition> vehiclePosition = *vehiclePositions.begin();

                pt::ScheduledService* scheduledService = vehiclePosition->getService();
                const pt::JourneyPattern* journeyPattern(scheduledService->getRoute());
				pt::CommercialLine* commercialLine(journeyPattern->getCommercialLine());
                
                util::RegistryKeyType commercialLineId(commercialLine->getKey());
                
                util::RegistryKeyType hashCode(23);
                hashCode = hashCode * 31 + commercialLineId;
                hashCode = hashCode * 31 + scheduledService->getKey();
                hashCode = hashCode * 31 + ALERT_TYPE_CALLBACKREQUEST;

                util::RegistryKeyType alertId(util::encodeUId(
                                                  AlertTableSync::TABLE.ID,
                                                  db::DBModule::GetNodeId(),
                                                  hashCode));
                
                Alert callbackRequestAlert(alertId);
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

            util::Log::GetInstance().debug("Processed callback requests alerts");
            
        }

    }
}
