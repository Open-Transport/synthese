#ifndef SYNTHESE_ENVLSSQL_ROADTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ROADTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Road;

/** Road SQLite table synchronizer.
	@ingroup m15
*/

class RoadTableSync : public ComponentTableSync
{
 public:

    RoadTableSync ();
    ~RoadTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};



/** Roads table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string ROADS_TABLE_NAME ("t015_roads");
static const std::string ROADS_TABLE_COL_NAME ("name");
static const std::string ROADS_TABLE_COL_CITYID ("city_id");
static const std::string ROADS_TABLE_COL_ROADTYPE ("road_type");
static const std::string ROADS_TABLE_COL_FAREID ("fare_id");
static const std::string ROADS_TABLE_COL_ALARMID ("alarm_id");
static const std::string ROADS_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
static const std::string ROADS_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
static const std::string ROADS_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
static const std::string ROADS_TABLE_COL_RESERVATIONRULEID ("reservation_rule_id");
// list of chunk ids




}

}
#endif

