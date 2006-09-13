#include "ContinuousServiceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "04_time/Schedule.h"

#include "15_env/ContinuousService.h"
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
using synthese::env::ContinuousService;

namespace synthese
{
namespace envlssql
{



ContinuousServiceTableSync::ContinuousServiceTableSync (Environment::Registry& environments,
							const std::string& triggerOverrideClause)
: ComponentTableSync (CONTINUOUSSERVICES_TABLE_NAME, environments, true, false, triggerOverrideClause)
{
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER, "TEXT", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_SCHEDULES, "TEXT", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_PATHID, "INTEGER", false);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_RANKINPATH, "INTEGER", false);
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

    uid pathId (Conversion::ToLongLong (rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_PATHID)));
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
	rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_RANKINPATH)));

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

    cs->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    cs->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    cs->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));

    environment.getContinuousServices ().add (cs);
}






void 
ContinuousServiceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    ContinuousService* cs = environment.getContinuousServices ().get (id);

    int serviceNumber (Conversion::ToInt (
        rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER)));

    std::string schedules (
	rows.getColumn (rowIndex, CONTINUOUSSERVICES_TABLE_COL_SCHEDULES));

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
    cs->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    cs->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    cs->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));

    // TODO : update in correcponding edge

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
