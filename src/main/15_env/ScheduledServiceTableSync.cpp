
/** ScheduledServiceTableSync class implementation.
	@file ScheduledServiceTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "ScheduledServiceTableSync.h"

#include "15_env/ScheduledService.h"
#include "15_env/Path.h"
#include "15_env/EnvModule.h"
#include "15_env/BikeCompliance.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/ReservationRule.h"
#include "15_env/LineTableSync.h"
#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/ServiceDateTableSync.h"

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "04_time/Schedule.h"
#include "04_time/Date.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>

#include <assert.h>
#include <set>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ScheduledService>::TABLE_NAME = "t016_scheduled_services";
		template<> const int SQLiteTableSyncTemplate<ScheduledService>::TABLE_ID = 16;
		template<> const bool SQLiteTableSyncTemplate<ScheduledService>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<ScheduledService>::load(ScheduledService* ss, const db::SQLiteResultSPtr& rows )
		{


		    uid id (rows->getLongLong (TABLE_COL_ID));
		    
		    int serviceNumber (rows->getInt (ScheduledServiceTableSync::COL_SERVICENUMBER));
		    
		    std::string schedules (rows->getText (ScheduledServiceTableSync::COL_SCHEDULES));

		    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

		    // Parse all schedules arrival#departure,arrival#departure...
		    boost::char_separator<char> sep1 (",");
		    tokenizer schedulesTokens (schedules, sep1);
		    
		    ScheduledService::Schedules departureSchedules;
		    ScheduledService::Schedules arrivalSchedules;
		    
		    for(tokenizer::iterator schedulesIter = schedulesTokens.begin();
				schedulesIter != schedulesTokens.end ();
				++schedulesIter
			){
				std::string arrDep (*schedulesIter);
				size_t sepPos = arrDep.find ("#");
				assert (sepPos != std::string::npos);
				
				std::string arrivalScheduleStr (arrDep.substr (0, sepPos));
				std::string departureScheduleStr (arrDep.substr (sepPos+1));
				
				// unnecessary : boost::trim (departureScheduleStr);
				// unnecessary : boost::trim (arrivalScheduleStr);
				
				if (departureScheduleStr.empty ())
				{
					assert (arrivalScheduleStr.empty () == false);
					departureScheduleStr = arrivalScheduleStr;
				}
				if (arrivalScheduleStr.empty ())
				{
					assert (departureScheduleStr.empty () == false);
					arrivalScheduleStr = departureScheduleStr;
				}
				
				Schedule departureSchedule (Schedule::FromString (departureScheduleStr));
				Schedule arrivalSchedule (Schedule::FromString (arrivalScheduleStr));
				
				departureSchedules.push_back (departureSchedule);
				arrivalSchedules.push_back (arrivalSchedule);
		    }
		    
		    assert (departureSchedules.size () > 0);
		    assert (arrivalSchedules.size () > 0);
		    assert (departureSchedules.size () == arrivalSchedules.size ());
		    
		    uid pathId (rows->getLongLong (ScheduledServiceTableSync::COL_PATHID));
		    
		    shared_ptr<Path> path = EnvModule::fetchPath (pathId);
		    assert (path != 0);
		    assert (path->getEdges ().size () == arrivalSchedules.size ());
		    
		    uid bikeComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_BIKECOMPLIANCEID));
		    
		    uid handicappedComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID));
		    
		    uid pedestrianComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID));
		    
		    uid reservationRuleId (rows->getLongLong (ScheduledServiceTableSync::COL_RESERVATIONRULEID));

		    ss->setPath(path.get());
		    ss->setServiceNumber(serviceNumber);
		    ss->setKey(id);
		    ss->setBikeCompliance (BikeCompliance::Get (bikeComplianceId).get());
		    ss->setHandicappedCompliance (HandicappedCompliance::Get (handicappedComplianceId).get());
		    ss->setPedestrianCompliance (PedestrianCompliance::Get (pedestrianComplianceId).get());
		    ss->setReservationRule (ReservationRule::Get (reservationRuleId).get());
		    ss->setDepartureSchedules(departureSchedules);
		    ss->setArrivalSchedules(arrivalSchedules);
		    ss->getPath()->addService(ss);
		}


		template<> void SQLiteTableSyncTemplate<ScheduledService>::save(ScheduledService* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string ScheduledServiceTableSync::COL_SERVICENUMBER ("service_number");
		const std::string ScheduledServiceTableSync::COL_SCHEDULES ("schedules");
		const std::string ScheduledServiceTableSync::COL_PATHID ("path_id");
		const std::string ScheduledServiceTableSync::COL_RANKINPATH ("rank_in_path");
		const std::string ScheduledServiceTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const std::string ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const std::string ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const std::string ScheduledServiceTableSync::COL_RESERVATIONRULEID ("reservation_rule_id");

		ScheduledServiceTableSync::ScheduledServiceTableSync()
			: SQLiteTableSyncTemplate<ScheduledService>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_SERVICENUMBER, "TEXT", true);
			addTableColumn (COL_SCHEDULES, "TEXT", true);
			addTableColumn (COL_PATHID, "INTEGER", false);
			addTableColumn (COL_BIKECOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_RESERVATIONRULEID, "INTEGER", true);
		}

		void ScheduledServiceTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    // Loop on each added row
		    while (rows->next ())
		    {
			if (ScheduledService::Contains(rows->getLongLong (TABLE_COL_ID)))
			{
			    boost::shared_ptr<ScheduledService> service(ScheduledService::GetUpdateable(rows->getLongLong (TABLE_COL_ID)));
			    service->getPath()->removeService(service.get());
			    load(service.get(), rows);
			}
			else
			{
			    ScheduledService* service(new ScheduledService);
			    load(service, rows);
			    service->store();
			}
			
		    }
		}
	    


		void ScheduledServiceTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
			    uid id = rows->getLongLong (TABLE_COL_ID);
			    if (ScheduledService::Contains(id))
			    {
				shared_ptr<ScheduledService> object = ScheduledService::GetUpdateable(id);
				object->getPath()->removeService(object.get());
				load(object.get(), rows);
			    }
			}
		}

		void ScheduledServiceTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
		    while (rows->next ())
		    {
			uid id = rows->getLongLong (TABLE_COL_ID);
			if (ScheduledService::Contains(id))
			{
			    shared_ptr<ScheduledService> object(ScheduledService::GetUpdateable(id));
			    object->getPath()->removeService(object.get());
			    ScheduledService::Remove(id);
			}
		    }
		}


		vector<shared_ptr<ScheduledService> > ScheduledServiceTableSync::search(
			CommercialLine* commercialLine
			, Date date
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByOriginTime
			, bool raisingOrder
		){
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME;
			if (commercialLine)
				query << " INNER JOIN " << LineTableSync::TABLE_NAME << " AS l ON l." << TABLE_COL_ID << "=" << COL_PATHID;
			query << " WHERE 1 ";
			if (commercialLine)
				query << " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << commercialLine->getKey();
			if (!date.isUnknown())
				query << " AND (EXISTS(SELECT * FROM " << ServiceDateTableSync::TABLE_NAME << " WHERE " << ServiceDateTableSync::COL_SERVICEID << "=" << TABLE_NAME << "." << TABLE_COL_ID << " AND " << ServiceDateTableSync::COL_DATE << "=" << date.toSQLString() << ")";
			query << " ORDER BY ";
			if (orderByOriginTime)
				query << COL_SCHEDULES << (raisingOrder ? " ASC" : " DESC");

			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<ScheduledService> > objects;
				while (rows->next ())
				{
					shared_ptr<ScheduledService> object ( new ScheduledService());
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		void ScheduledServiceTableSync::afterFirstSync( SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync )
		{
/*
		    // Lines
		    for (Line::Registry::const_iterator it = EnvModule::getLines().begin(); it != EnvModule::getLines().end(); it++)
		    {
			shared_ptr<Line> line = EnvModule::getLines().getUpdateable(it->first);
			line ->updateScheduleIndexes();
		    }

		    
		    // Roads
			for (Road::Registry::const_iterator it = EnvModule::getRoads().begin(); it != EnvModule::getLines().end(); it++)
			{
			(*it)->updateSchedulesIndexes();
			}
*/

		}
	}
}

