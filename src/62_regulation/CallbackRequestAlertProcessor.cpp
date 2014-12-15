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

#include "VehicleCallTableSync.hpp"
#include "VehiclePositionTableSync.hpp"
#include "DB.hpp"
#include "Env.h"
#include "ScheduledService.h"

using namespace std;

namespace synthese
{
    namespace regulation
    {

        CallbackRequestAlertProcessor::CallbackRequestAlertProcessor()
            : AlertProcessor()
        {
        }

        
        void
        CallbackRequestAlertProcessor::processAlerts()
        {
            util::Log::GetInstance().info("MJ callback request alert processor");
            /*
			VehicleCallTableSync::SearchResult vehicleCalls(
				VehicleCallTableSync::Search(
					_getEnv(),
					boost::optional<util::RegistryKeyType>()
                    )
                );
            */
            
			stringstream query;
			query <<
				" SELECT * FROM " << vehicle::VehicleCallTableSync::TABLE.NAME << " vc " <<
				" INNER JOIN " << vehicle::VehiclePositionTableSync::TABLE.NAME << " vp " <<
                " ON vc.vehicle_id" /*<< vehicle::VehicleCall<Vehicle>::FIELD.name*/ << " = vp." << vehicle::VehiclePositionTableSync::COL_VEHICLE_ID;
            util::Log::GetInstance().info(query.str());
            db::DBResultSPtr result(db::DBModule::GetDB()->execQuery(query.str()));
            while(result->next())
            {
                int serviceId(result->get<util::RegistryKeyType>("service_id"));
                boost::shared_ptr<const pt::ScheduledService> scheduledService(util::Env::GetOfficialEnv().get<pt::ScheduledService>(serviceId));
                const pt::JourneyPattern* journeyPattern(scheduledService->getRoute());
				const pt::CommercialLine* commercialLine(journeyPattern->getCommercialLine());
                
                util::Log::GetInstance().info("journeyPattern " + journeyPattern->getKey());
                
            }
            

            // pour chq vehicle call

            // recuperer sa position (t072_vehicle_positions /
            // vehicle_id)

            // depuis sa position recuperer le scheduledservice

            // depuis le schedserv recuperer journey pattern puis
            // commercial line

            // ok on a toutes les infos pour ecrire l'alerte!
            // TODO : lundi !!
            
        }

    }
}
