
/** ContinuousServiceTableSync class implementation.
	@file ContinuousServiceTableSync.cpp

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

#include "ContinuousServiceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "04_time/Schedule.h"

#include "15_env/ContinuousService.h"
#include "15_env/Path.h"
#include "15_env/Point.h"
#include "15_env/EnvModule.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>

#include <assert.h>



using synthese::util::Conversion;
using synthese::time::Schedule;

namespace synthese
{
	using namespace db;

namespace env
{



ContinuousServiceTableSync::ContinuousServiceTableSync ()
: ComponentTableSync (CONTINUOUSSERVICES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER, "TEXT", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_SCHEDULES, "TEXT", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_PATHID, "INTEGER", false);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_RANGE, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_MAXWAITINGTIME, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_BIKECOMPLIANCEID, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
}



ContinuousServiceTableSync::~ContinuousServiceTableSync ()
{

}

    


void 
ContinuousServiceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
				   synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    if (environment.getContinuousServices ().contains (id)) return;

    int serviceNumber (Conversion::ToInt (
        rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER)));

    std::string schedules (
	rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SCHEDULES));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // Parse all schedules arrival#departure,arrival#departure...
    boost::char_separator<char> sep1 (",");
    tokenizer schedulesTokens (schedules, sep1);
    
    std::vector<synthese::time::Schedule> departureSchedules;
    std::vector<synthese::time::Schedule> arrivalSchedules;

    for (tokenizer::iterator schedulesIter = schedulesTokens.begin();
	 schedulesIter != schedulesTokens.end (); ++schedulesIter)
    {
	std::string arrDep (*schedulesIter);
	size_t sepPos = arrDep.find ("#");
	assert (sepPos != std::string::npos);

	std::string departureScheduleStr (arrDep.substr (0, sepPos));
	std::string arrivalScheduleStr (arrDep.substr (sepPos+1));

	boost::trim (departureScheduleStr);
	boost::trim (arrivalScheduleStr);
	
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

    uid pathId (Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_PATHID)));

	Path* path = EnvModule::fetchPath (pathId);
    assert (path != 0);
    assert (path->getEdges ().size () == arrivalSchedules.size ());

    int range (Conversion::ToInt (
		   rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_RANGE)));
    
    int maxWaitingTime (Conversion::ToInt (
			    rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_MAXWAITINGTIME)));

    uid bikeComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_BIKECOMPLIANCEID)));

    uid handicappedComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));

    uid pedestrianComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID)));

    ContinuousService* cs = new synthese::env::ContinuousService (id, serviceNumber, 
								  path, 
								  departureSchedules.at (0),
								  range, maxWaitingTime);

	cs->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId));
	cs->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId));
	cs->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId));

    path->addService (cs, departureSchedules, arrivalSchedules);
    environment.getContinuousServices ().add (cs);
}






void 
ContinuousServiceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    ContinuousService* cs = environment.getContinuousServices ().get (id);

    // Remove old service
	Path* path = EnvModule::fetchPath (cs->getPath ()->getId ());
    path->removeService (cs);

    int serviceNumber (Conversion::ToInt (
        rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER)));

    std::string schedules (
	rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SCHEDULES));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // Parse all schedules arrival#departure,arrival#departure...
    boost::char_separator<char> sep1 (",");
    tokenizer schedulesTokens (schedules, sep1);
    
    std::vector<synthese::time::Schedule> departureSchedules;
    std::vector<synthese::time::Schedule> arrivalSchedules;

    for (tokenizer::iterator schedulesIter = schedulesTokens.begin();
	 schedulesIter != schedulesTokens.end (); ++schedulesIter)
    {
	std::string arrDep (*schedulesIter);
	size_t sepPos = arrDep.find ("#");
	assert (sepPos != std::string::npos);

	std::string departureScheduleStr (arrDep.substr (0, sepPos));
	std::string arrivalScheduleStr (arrDep.substr (sepPos+1));

	boost::trim (departureScheduleStr);
	boost::trim (arrivalScheduleStr);
	
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

    assert (cs->getPath ()->getEdges ().size () == arrivalSchedules.size ());

    int range (Conversion::ToInt (
		   rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_RANGE)));
    
    int maxWaitingTime (Conversion::ToInt (
			    rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_MAXWAITINGTIME)));

    uid bikeComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_BIKECOMPLIANCEID)));

    uid handicappedComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));

    uid pedestrianComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID)));

    cs->setServiceNumber (serviceNumber);
    cs->setDepartureSchedule (departureSchedules.at (0));
    cs->setRange (range);
    cs->setMaxWaitingTime (range);
	cs->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId));
	cs->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId));
	cs->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId));

    path->addService (cs, departureSchedules, arrivalSchedules);
}





void 
ContinuousServiceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getContinuousServices ().remove (id);
}















}

}

