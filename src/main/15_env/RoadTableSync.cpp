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


using namespace boost;
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

    shared_ptr<City> city = environment.getCities ().getUpdateable (cityId);
    shared_ptr<Road> road (
					  new synthese::env::Road (id, name, 
					  					  city.get(), 
					  					  roadType))
					  ;
	road->setFare (EnvModule::getFares ().get (fareId).get());
//    road->setAlarm (environment.getAlarms ().get (alarmId));
	road->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId).get());
	road->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId).get());
	road->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId).get());
	road->setReservationRule (EnvModule::getReservationRules ().get (reservationRuleId).get()); 
    
    environment.getRoads ().add (road);
    city->getRoadsMatcher ().add (road->getName (), road.get());

}






void 
RoadTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    shared_ptr<Road> road = environment.getRoads ().getUpdateable(id);

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

    shared_ptr<City> city = environment.getCities ().getUpdateable (road->getCity ()->getKey ());
    city->getRoadsMatcher ().remove (road->getName ());

    road->setName (name);
    road->setType (roadType);

	road->setFare (EnvModule::getFares ().get (fareId).get());
//    road->setAlarm (environment.getAlarms ().get (alarmId));
	road->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId).get());
	road->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId).get());
	road->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId).get());
	road->setReservationRule (EnvModule::getReservationRules ().get (reservationRuleId).get()); 
    
    city->getRoadsMatcher ().add (road->getName (), road.get());

}





void 
RoadTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    shared_ptr<Road> road = environment.getRoads ().getUpdateable(id);
    shared_ptr<City> city = environment.getCities ().getUpdateable (road->getCity ()->getKey ());
    city->getRoadsMatcher ().remove (road->getName ());

    environment.getRoads ().remove (id);

}















}

}

