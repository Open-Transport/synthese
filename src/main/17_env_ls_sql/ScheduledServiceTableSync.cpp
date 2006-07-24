#include "ScheduledServiceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "04_time/Schedule.h"

#include "15_env/ScheduledService.h"
#include "15_env/Path.h"
#include "15_env/Point.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>

#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::time::Schedule;
using synthese::env::Environment;
using synthese::env::Path;
using synthese::env::Point;
using synthese::env::ScheduledService;

namespace synthese
{
namespace envlssql
{



ScheduledServiceTableSync::ScheduledServiceTableSync (Environment::Registry& environments)
: ComponentTableSync (SCHEDULEDSERVICES_TABLE_NAME, environments)
{
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_SERVICENUMBER, "TEXT");
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_SCHEDULES, "TEXT");
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_PATHID, "INTEGER");
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_RANKINPATH, "INTEGER");
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_BIKECOMPLIANCEID, "INTEGER");
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID, "INTEGER");
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID, "INTEGER");
    addTableColumn (SCHEDULEDSERVICES_TABLE_COL_RESERVATIONRULEID, "INTEGER");
}



ScheduledServiceTableSync::~ScheduledServiceTableSync ()
{

}

    


void 
ScheduledServiceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    std::string serviceNumber (
	rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_SERVICENUMBER));

    std::string schedules (
	rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_SCHEDULES));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // Parse all schedules arrival#departure,arrival#departure...
    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 ("#");
    tokenizer schedulesTokens (schedules, sep1);
    
    std::vector<synthese::time::Schedule> departureSchedules;
    std::vector<synthese::time::Schedule> arrivalSchedules;

    for (tokenizer::iterator schedulesIter = schedulesTokens.begin();
	 schedulesIter != schedulesTokens.end (); ++schedulesIter)
    {
	tokenizer schedulesTokens2 (*schedulesIter, sep2);
	tokenizer::iterator schedulesIter2 = schedulesTokens2.begin();
	
	departureSchedules.push_back (Schedule::FromString (*schedulesIter2));
	arrivalSchedules.push_back (Schedule::FromString (*(++schedulesIter2)));
    }
    
    assert (departureSchedules.size () > 0);
    assert (arrivalSchedules.size () > 0);

    uid pathId (Conversion::ToLongLong (rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_PATHID)));
    int tableId = synthese::util::decodeTableId (pathId);

    Path* path = 0;
    if (tableId == ParseTableId (ROADS_TABLE_NAME))
    {
	path = environment.getRoads ().get (pathId);
    }
    else if (tableId == ParseTableId (LINES_TABLE_NAME ))
    {
	path = environment.getLines ().get (pathId);
    }
    assert (path != 0);

    int rankInPath (Conversion::ToInt (
	rows.getColumn (rowIndex, SCHEDULEDSERVICES_TABLE_COL_RANKINPATH)));

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

    environment.getScheduledServices ().add (ss, false);
}






void 
ScheduledServiceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
}





void 
ScheduledServiceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
}















}

}
