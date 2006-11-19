#include "ScheduledServiceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "04_time/Schedule.h"

#include "15_env/ScheduledService.h"
#include "15_env/Path.h"
#include "15_env/Point.h"
#include "15_env/ContinuousServiceTableSync.h"

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



ScheduledServiceTableSync::ScheduledServiceTableSync ()
: ComponentTableSync (SCHEDULEDSERVICES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_SERVICENUMBER, "TEXT", true);
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_SCHEDULES, "TEXT", true);
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_PATHID, "INTEGER", false);
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_BIKECOMPLIANCEID, "INTEGER", true);
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_RESERVATIONRULEID, "INTEGER", true);
}



ScheduledServiceTableSync::~ScheduledServiceTableSync ()
{

}

    


void 
ScheduledServiceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    if (environment.getScheduledServices ().contains (id)) return;

    int serviceNumber (Conversion::ToInt (
        rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER)));

    std::string schedules (
	rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_SCHEDULES));

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

    uid pathId (Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_PATHID)));

    Path* path = environment.fetchPath (pathId);
    assert (path != 0);
    assert (path->getEdges ().size () == arrivalSchedules.size ());

    uid bikeComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_BIKECOMPLIANCEID)));

    uid handicappedComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));

    uid pedestrianComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID)));

    uid reservationRuleId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_RESERVATIONRULEID)));

    ScheduledService* ss = new synthese::env::ScheduledService (id, serviceNumber, 
								path, 
								departureSchedules.at (0));

    ss->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    ss->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    ss->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    ss->setReservationRule (environment.getReservationRules ().get (reservationRuleId)); 

    path->addService (ss, departureSchedules, arrivalSchedules);
    environment.getScheduledServices ().add (ss);
}






void 
ScheduledServiceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    ScheduledService* ss = environment.getScheduledServices ().get (id);

    // Remove old service
    Path* path = environment.fetchPath (ss->getPath ()->getId ());
    path->removeService (ss);

    int serviceNumber (Conversion::ToInt (
			   rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER)));

    std::string schedules (
	rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_SCHEDULES));

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

    assert (ss->getPath ()->getEdges ().size () == arrivalSchedules.size ());

    uid bikeComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_BIKECOMPLIANCEID)));

    uid handicappedComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));

    uid pedestrianComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID)));

    uid reservationRuleId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_RESERVATIONRULEID)));

    ss->setServiceNumber (serviceNumber);
    ss->setDepartureSchedule (departureSchedules.at (0));
    ss->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    ss->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    ss->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    ss->setReservationRule (environment.getReservationRules ().get (reservationRuleId)); 
    
    path->addService (ss, departureSchedules, arrivalSchedules);
    
}





void 
ScheduledServiceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getScheduledServices ().remove (id);
}















}

}
