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


            //const VehicleCall::Registry& registry = Env::GetOfficialEnv().getRegistry<VehicleCall>();

			BOOST_FOREACH(const VehicleCall::Registry::value_type& itVehicleCall, //registry)
                          Env::GetOfficialEnv().getRegistry<VehicleCall>())
            {
                
                boost::shared_ptr<const VehiclePosition> vehiclePosition =
                    Env::GetOfficialEnv().getRegistry<VehiclePosition>().get(itVehicleCall.second->get<synthese::Vehicle>());

            }
                
			stringstream query;
			query <<
				" SELECT * FROM " << vehicle::VehicleCallTableSync::TABLE.NAME << " vc " <<
				" INNER JOIN " << vehicle::VehiclePositionTableSync::TABLE.NAME << " vp " <<
                " ON vc.vehicle_id" /*<< vehicle::VehicleCall<Vehicle>::FIELD.name*/ << " = vp." << vehicle::VehiclePositionTableSync::COL_VEHICLE_ID;

            db::DBResultSPtr result(db::DBModule::GetDB()->execQuery(query.str()));
            while(result->next())
            {
                util::RegistryKeyType serviceId(result->get<util::RegistryKeyType>("service_id"));

                boost::shared_ptr<pt::ScheduledService> scheduledService(
                    util::Env::GetOfficialEnv().getEditable<pt::ScheduledService>(serviceId));

                const pt::JourneyPattern* journeyPattern(scheduledService->getRoute());
				pt::CommercialLine* commercialLine(journeyPattern->getCommercialLine());

                /*std::cerr << "Found vehicle call for service id = " << serviceId
                          << " journey pattern id = " << journeyPattern->getKey() << " ; commercial line id  = "
                          << commercialLine->getKey() << std::endl;*/

                util::RegistryKeyType commercialLineId(commercialLine->getKey());

                util::RegistryKeyType hashCode(23);
                hashCode = hashCode * 31 + commercialLineId;
                hashCode = hashCode * 31 + serviceId;
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
                extraDataPM.insert("priority", result->get<bool>("priority"));
                std::stringstream extraDataStream;
                extraDataPM.outputJSON(extraDataStream, "extraData");

                callbackRequestAlert.set<ExtraData>(extraDataStream.str());

                AlertTableSync::Save(&callbackRequestAlert);
            }
            
            util::Log::GetInstance().debug("Processed callback requests alerts");
            
        }

    }
}
