#include "RoadTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/EnvModule.h"
#include "15_env/Road.h"
#include "15_env/City.h"
#include "15_env/Point.h"
#include "15_env/RoadChunkTableSync.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>



using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



RoadTableSync::RoadTableSync ()
: ComponentTableSync (ROADS_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
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

    if (environment.getRoads ().contains (id)) return;

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

    City* city = environment.getCities ().get (cityId);
    Road* road = new synthese::env::Road (id, name, 
					  city, 
					  roadType);
	road->setFare (EnvModule::getFares ().get (fareId));
//    road->setAlarm (environment.getAlarms ().get (alarmId));
	road->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId));
	road->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId));
	road->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId));
	road->setReservationRule (EnvModule::getReservationRules ().get (reservationRuleId)); 
    
    environment.getRoads ().add (road);
    city->getRoadsMatcher ().add (road->getName (), road);

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

    City* city = environment.getCities ().get (road->getCity ()->getKey ());
    city->getRoadsMatcher ().remove (road->getName ());

    road->setName (name);
    road->setType (roadType);

	road->setFare (EnvModule::getFares ().get (fareId));
//    road->setAlarm (environment.getAlarms ().get (alarmId));
	road->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId));
	road->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId));
	road->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId));
	road->setReservationRule (EnvModule::getReservationRules ().get (reservationRuleId)); 
    
    city->getRoadsMatcher ().add (road->getName (), road);

}





void 
RoadTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    Road* road = environment.getRoads ().get (id);
    City* city = environment.getCities ().get (road->getCity ()->getKey ());
    city->getRoadsMatcher ().remove (road->getName ());

    environment.getRoads ().remove (id);

}















}

}

