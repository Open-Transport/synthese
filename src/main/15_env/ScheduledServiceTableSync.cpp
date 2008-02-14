
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

#include "15_env/Path.h"
#include "15_env/EnvModule.h"
#include "15_env/BikeComplianceTableSync.h"
#include "15_env/HandicappedComplianceTableSync.h"
#include "15_env/PedestrianComplianceTableSync.h"
#include "15_env/ReservationRuleTableSync.h"
#include "15_env/LineTableSync.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/ServiceDateTableSync.h"

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "04_time/Schedule.h"
#include "04_time/Date.h"

#include <boost/tokenizer.hpp>

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

	template<> const string util::FactorableTemplate<SQLiteTableSync,ScheduledServiceTableSync>::FACTORY_KEY("15.60.03 Scheduled services");

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<ScheduledServiceTableSync>::TABLE_NAME = "t016_scheduled_services";
		template<> const int SQLiteTableSyncTemplate<ScheduledServiceTableSync>::TABLE_ID = 16;
		template<> const bool SQLiteTableSyncTemplate<ScheduledServiceTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>::load(ScheduledService* ss, const db::SQLiteResultSPtr& rows )
		{


		    uid id (rows->getLongLong (TABLE_COL_ID));
		    
		    int serviceNumber (rows->getInt (ScheduledServiceTableSync::COL_SERVICENUMBER));

			uid pathId(rows->getLongLong(ScheduledServiceTableSync::COL_PATHID));
		    
		    string schedules (rows->getText (ScheduledServiceTableSync::COL_SCHEDULES));

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
				string arrDep (*schedulesIter);
				size_t sepPos = arrDep.find ("#");
				assert (sepPos != string::npos);
				
				string arrivalScheduleStr (arrDep.substr (0, sepPos));
				string departureScheduleStr (arrDep.substr (sepPos+1));
				
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
		    
		    
		    ss->setServiceNumber(serviceNumber);
		    ss->setKey(id);
		    ss->setDepartureSchedules(departureSchedules);
		    ss->setArrivalSchedules(arrivalSchedules);
			ss->setTeam(rows->getText(ScheduledServiceTableSync::COL_TEAM));
			ss->setPathId(pathId);
		}



		template<> void SQLiteDirectTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>::_link(ScheduledService* ss, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			uid pathId (rows->getLongLong (ScheduledServiceTableSync::COL_PATHID));

			Path* path = LineTableSync::GetUpdateable(pathId,ss,temporary);
			
			assert (path);
//			assert (path->getEdges ().size () == ss-> arrivalSchedules.size ());

			uid bikeComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_BIKECOMPLIANCEID));

			uid handicappedComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID));

			uid pedestrianComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID));

			uid reservationRuleId (rows->getLongLong (ScheduledServiceTableSync::COL_RESERVATIONRULEID));

			ss->setPath(path);
			ss->setBikeCompliance (BikeComplianceTableSync::Get (bikeComplianceId,ss,true,temporary));
			ss->setHandicappedCompliance (HandicappedComplianceTableSync::Get (handicappedComplianceId,ss,true,temporary));
			ss->setPedestrianCompliance (PedestrianComplianceTableSync::Get (pedestrianComplianceId,ss,true,temporary));
			ss->setReservationRule (ReservationRuleTableSync::Get (reservationRuleId,ss,true,temporary));

			path->addService(ss);
		}

		template<> void SQLiteDirectTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>::_unlink(ScheduledService* ss)
		{
			ss->getPath()->removeService(ss);
		}

		template<> void SQLiteDirectTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>::save(ScheduledService* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
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
		const string ScheduledServiceTableSync::COL_SERVICENUMBER ("service_number");
		const string ScheduledServiceTableSync::COL_SCHEDULES ("schedules");
		const string ScheduledServiceTableSync::COL_PATHID ("path_id");
		const string ScheduledServiceTableSync::COL_RANKINPATH ("rank_in_path");
		const string ScheduledServiceTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const string ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const string ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const string ScheduledServiceTableSync::COL_RESERVATIONRULEID ("reservation_rule_id");
		const string ScheduledServiceTableSync::COL_TEAM("team");

		ScheduledServiceTableSync::ScheduledServiceTableSync()
			: SQLiteRegistryTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_SERVICENUMBER, "TEXT", true);
			addTableColumn (COL_SCHEDULES, "TEXT", true);
			addTableColumn (COL_PATHID, "INTEGER", false);
			addTableColumn (COL_BIKECOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_RESERVATIONRULEID, "INTEGER", true);
			addTableColumn (COL_TEAM, "TEXT");
		}



		vector<shared_ptr<ScheduledService> > ScheduledServiceTableSync::search(
			const CommercialLine* commercialLine
			, Date date
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByOriginTime
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME;
			if (commercialLine)
				query << " INNER JOIN " << LineTableSync::TABLE_NAME << " AS l ON l." << TABLE_COL_ID << "=" << COL_PATHID;
			if (!date.isUnknown())
				query << " INNER JOIN " << ServiceDateTableSync::TABLE_NAME << " AS d ON d." << ServiceDateTableSync::COL_SERVICEID << "=" << TABLE_NAME << "." << TABLE_COL_ID;
			query << " WHERE 1 ";
			if (commercialLine)
				query << " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << commercialLine->getKey();
			if (!date.isUnknown())
				query << " AND d." << ServiceDateTableSync::COL_DATE << "=" << date.toSQLString();
			if (!date.isUnknown())
				query << " GROUP BY " << TABLE_NAME << "." << TABLE_COL_ID;
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

		void ScheduledServiceTableSync::afterFirstSync( SQLite* sqlite,  SQLiteSync* sync )
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

