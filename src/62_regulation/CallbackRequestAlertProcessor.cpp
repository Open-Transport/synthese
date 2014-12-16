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
#include "SelectQuery.hpp"

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
            for (int i = 0 ; i < result->getNbColumns(); ++i) {
                util::Log::GetInstance().info("-- col " + result->getColumnName(i));                
            }
            while(result->next())
            {
                util::RegistryKeyType serviceId(result->get<util::RegistryKeyType>("service_id"));
                std::cerr << "service id " << serviceId << std::endl;
                boost::shared_ptr<const pt::ScheduledService> scheduledService(util::Env::GetOfficialEnv().get<pt::ScheduledService>(serviceId));
                std::cerr << "TOTO1" << std::endl;
                const pt::JourneyPattern* journeyPattern(scheduledService->getRoute());
                /*
                DBModule::LoadObjects(journeyPattern->getLinkedObjectsIds(*result), env, linkLevel);
                obj->loadFromRecord(*rows, env);
                */
                
                std::cerr << "TOTO2" << journeyPattern << " " << std::endl;
				const pt::CommercialLine* commercialLine(journeyPattern->getCommercialLine());
                std::cerr << "TOTO3" << journeyPattern->getKey() << std::endl;
                
                std::cerr << "Found vehicle call for service id = " << serviceId << " journey pattern id = " << journeyPattern->getKey() << " ; commercial line id  = " << commercialLine << std::endl;



                
/*
			DBModule::LoadObjects(obj->getLinkedObjectsIds(*rows), env, linkLevel);
			obj->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				obj->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
*/  

                
            }
            
/*
			SelectQuery<AlarmObjectLinkTableSync> query;
			query.addTableField(AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualJoin<AlarmTableSync>(TABLE_COL_ID, AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualOtherJoin<ScenarioTableSync, AlarmTableSync>(TABLE_COL_ID, AlarmTableSync::COL_SCENARIO_ID);
			query.addWhereField(AlarmObjectLinkTableSync::COL_OBJECT_ID, screenId);
			query.addWhereFieldOther<ScenarioTableSync>(ScenarioTableSync::COL_ENABLED, 1);
			query.addWhere(
				SQLSingleOperatorExpression::Get(
					SQLSingleOperatorExpression::OP_NOT,
					FieldExpression::Get(
						ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_IS_TEMPLATE
			)	)	);
*/
                
                // TODO : pb commercial line not loaded . review query
                // to use selectquery with proper joins

                // probably the right way of doing it is implementing
                // a search on vehiclecall table sync, adding proper
                // joins to get vehicle, vehicle position, service,
                // line, commercial line

/*            db::SelectQuery<vehicle::VehicleCallTableSync> squery;
            squery.addTableAndEqualJoin<vehicle::VehiclePositionTableSync>("vehicle_id",
                                                                           vehicle::VehiclePositionTableSync::COL_VEHICLE_ID);
            squery.addTableField(vehicle::VehiclePositionTableSync::COL_SERVICE_ID);
*/          
/*            
            query.addTableField(AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualJoin<AlarmTableSync>(TABLE_COL_ID, AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualOtherJoin<ScenarioTableSync, AlarmTableSync>(TABLE_COL_ID, AlarmTableSync::COL_SCENARIO_ID);
			query.addWhereField(AlarmObjectLinkTableSync::COL_OBJECT_ID, screenId);
			query.addWhereFieldOther<ScenarioTableSync>(ScenarioTableSync::COL_ENABLED, 1);
			query.addWhere(
				SQLSingleOperatorExpression::Get(
					SQLSingleOperatorExpression::OP_NOT,
					FieldExpression::Get(
						ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_IS_TEMPLATE
			)	)	);
*/
            // std::cerr << "query " << squery.toString() << std::endl;
            
            





                

                
                

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
