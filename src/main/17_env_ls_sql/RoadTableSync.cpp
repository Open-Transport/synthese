#include "RoadTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Road.h"
#include "15_env/Point.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;
using synthese::env::Point;
using synthese::env::Road;

namespace synthese
{
namespace envlssql
{



RoadTableSync::RoadTableSync (Environment::Registry& environments)
: ComponentTableSync (ROADS_TABLE_NAME, environments, true, false)
{
    addTableColumn (ROADS_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (ROADS_TABLE_COL_CITYID, "INTEGER", false);
    addTableColumn (ROADS_TABLE_COL_ROADTYPE, "INTEGER", true);
    addTableColumn (ROADS_TABLE_COL_FAREID, "INTEGER", true);
    addTableColumn (ROADS_TABLE_COL_ALARMID, "INTEGER", true);
    addTableColumn (ROADS_TABLE_COL_BIKECOMPLIANCEID, "INTEGER", true);
    addTableColumn (ROADS_TABLE_COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
    addTableColumn (ROADS_TABLE_COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
    addTableColumn (ROADS_TABLE_COL_RESERVATIONRULEID, "INTEGER", true);

    addTableColumn (ROADCHUNKS_TABLE_COL_VIAPOINTS, "TEXT", true);
}



RoadTableSync::~RoadTableSync ()
{

}

    


void 
RoadTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    std::string name (
	rows.getColumn (rowIndex, ROADS_TABLE_COL_NAME));

    uid cityId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_CITYID)));

    Road::RoadType roadType = (Road::RoadType)
	Conversion::ToInt (rows.getColumn (rowIndex, ROADS_TABLE_COL_ROADTYPE));
    
    uid fareId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_FAREID)));

    uid alarmId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_ALARMID)));

    uid bikeComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_BIKECOMPLIANCEID)));

    uid handicappedComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));

    uid pedestrianComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_PEDESTRIANCOMPLIANCEID)));

    uid reservationRuleId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_RESERVATIONRULEID)));

    Road* road = new synthese::env::Road (id, name, 
					  environment.getCities ().get (cityId), 
					  roadType);
    road->setFare (environment.getFares ().get (fareId));
    road->setAlarm (environment.getAlarms ().get (alarmId));
    road->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    road->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    road->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    road->setReservationRule (environment.getReservationRules ().get (reservationRuleId)); 
    
    environment.getRoads ().add (road, false);
}






void 
RoadTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    Road* road = environment.getRoads ().get (id);

    std::string name (
	rows.getColumn (rowIndex, ROADS_TABLE_COL_NAME));

    Road::RoadType roadType = (Road::RoadType)
	Conversion::ToInt (rows.getColumn (rowIndex, ROADS_TABLE_COL_ROADTYPE));
    
    uid fareId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_FAREID)));

    uid alarmId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_ALARMID)));

    uid bikeComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_BIKECOMPLIANCEID)));

    uid handicappedComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));

    uid pedestrianComplianceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_PEDESTRIANCOMPLIANCEID)));

    uid reservationRuleId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADS_TABLE_COL_RESERVATIONRULEID)));

    road->setName (name);
    road->setType (roadType);

    road->setFare (environment.getFares ().get (fareId));
    road->setAlarm (environment.getAlarms ().get (alarmId));
    road->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    road->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    road->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    road->setReservationRule (environment.getReservationRules ().get (reservationRuleId)); 
    
    environment.getRoads ().add (road, false);


}





void 
RoadTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getRoads ().remove (id);

}















}

}
