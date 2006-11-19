#ifndef SYNTHESE_ENVLSSQL_LINETABLESYNC_H
#define SYNTHESE_ENVLSSQL_LINETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
    class LineTableSync;

/** Line SQLite table synchronizer.
	@ingroup m15
*/
class LineTableSync : public ComponentTableSync
{
 public:

    LineTableSync ();
    ~LineTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};


/** Lines table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string LINES_TABLE_NAME ("t009_lines");
static const std::string LINES_TABLE_COL_TRANSPORTNETWORKID ("transport_network_id");
static const std::string LINES_TABLE_COL_AXISID ("axis_id");
// 	    static const std::string LINES_TABLE_COL_CALENDARID ("calendar_id");   // Non car calculé
static const std::string LINES_TABLE_COL_NAME ("name");
static const std::string LINES_TABLE_COL_SHORTNAME ("short_name");
static const std::string LINES_TABLE_COL_LONGNAME ("long_name");
static const std::string LINES_TABLE_COL_COLOR ("color");
static const std::string LINES_TABLE_COL_STYLE ("style");
static const std::string LINES_TABLE_COL_IMAGE ("image");
static const std::string LINES_TABLE_COL_TIMETABLENAME ("timetable_name");
static const std::string LINES_TABLE_COL_DIRECTION ("direction");
static const std::string LINES_TABLE_COL_ISWALKINGLINE ("is_walking_line");
static const std::string LINES_TABLE_COL_USEINDEPARTUREBOARDS ("use_in_departure_boards");
static const std::string LINES_TABLE_COL_USEINTIMETABLES ("use_in_timetables");
static const std::string LINES_TABLE_COL_USEINROUTEPLANNING ("use_in_routeplanning");
static const std::string LINES_TABLE_COL_ROLLINGSTOCKID ("rolling_stock_id");
static const std::string LINES_TABLE_COL_FAREID ("fare_id");
static const std::string LINES_TABLE_COL_ALARMID ("alarm_id");
static const std::string LINES_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
static const std::string LINES_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
static const std::string LINES_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
static const std::string LINES_TABLE_COL_RESERVATIONRULEID ("reservation_rule_id");




}

}
#endif
