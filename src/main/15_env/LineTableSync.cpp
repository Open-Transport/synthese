#include "LineTableSync.h"

#include "01_util/RGBColor.h"
#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Line.h"

#include <sqlite/sqlite3.h>
#include <boost/logic/tribool.hpp>

#include <assert.h>

using boost::logic::tribool;

using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



LineTableSync::LineTableSync ()
: ComponentTableSync (LINES_TABLE_NAME, true, false, TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (LINES_TABLE_COL_TRANSPORTNETWORKID, "INTEGER", false);
    addTableColumn (LINES_TABLE_COL_AXISID, "INTEGER", false);
    addTableColumn (LINES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (LINES_TABLE_COL_SHORTNAME, "TEXT", true);
    addTableColumn (LINES_TABLE_COL_LONGNAME, "TEXT", true);
    addTableColumn (LINES_TABLE_COL_COLOR, "TEXT", true);

    addTableColumn (LINES_TABLE_COL_STYLE, "TEXT", true);
    addTableColumn (LINES_TABLE_COL_IMAGE, "TEXT", true);
    addTableColumn (LINES_TABLE_COL_TIMETABLENAME, "TEXT", true);
    addTableColumn (LINES_TABLE_COL_DIRECTION, "TEXT", true);
    addTableColumn (LINES_TABLE_COL_ISWALKINGLINE, "BOOLEAN", true);
    addTableColumn (LINES_TABLE_COL_USEINDEPARTUREBOARDS, "BOOLEAN", true);
    addTableColumn (LINES_TABLE_COL_USEINTIMETABLES, "BOOLEAN", true);
    addTableColumn (LINES_TABLE_COL_USEINROUTEPLANNING, "BOOLEAN", true);

    addTableColumn (LINES_TABLE_COL_ROLLINGSTOCKID, "INTEGER", true);
    addTableColumn (LINES_TABLE_COL_FAREID, "INTEGER", true);
    addTableColumn (LINES_TABLE_COL_ALARMID, "INTEGER", true);
    addTableColumn (LINES_TABLE_COL_BIKECOMPLIANCEID, "INTEGER", true);
    addTableColumn (LINES_TABLE_COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
    addTableColumn (LINES_TABLE_COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
    addTableColumn (LINES_TABLE_COL_RESERVATIONRULEID, "INTEGER", true);

}


LineTableSync::~LineTableSync ()
{
}


void 
LineTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    if (environment.getLines ().contains (id)) return;

    uid transportNetworkId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_TRANSPORTNETWORKID)));
    uid axisId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_AXISID)));
    
    std::string name (
	rows.getColumn (rowIndex, LINES_TABLE_COL_NAME));
    std::string shortName (
	rows.getColumn (rowIndex, LINES_TABLE_COL_SHORTNAME));
    std::string longName (
	rows.getColumn (rowIndex, LINES_TABLE_COL_LONGNAME));
    std::string color (
	rows.getColumn (rowIndex, LINES_TABLE_COL_COLOR));
    std::string style (
	rows.getColumn (rowIndex, LINES_TABLE_COL_STYLE));
    std::string image (
	rows.getColumn (rowIndex, LINES_TABLE_COL_IMAGE));
    std::string timetableName (
	rows.getColumn (rowIndex, LINES_TABLE_COL_TIMETABLENAME));
    std::string direction (
	rows.getColumn (rowIndex, LINES_TABLE_COL_DIRECTION));

    bool isWalkingLine (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_ISWALKINGLINE)));
    bool useInDepartureBoards (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_USEINDEPARTUREBOARDS)));
    bool useInTimetables (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_USEINTIMETABLES)));
    bool useInRoutePlanning (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_USEINROUTEPLANNING)));

    uid rollingStockId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_ROLLINGSTOCKID)));
    uid fareId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_FAREID)));
    uid alarmId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_ALARMID)));
    uid bikeComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_BIKECOMPLIANCEID)));
    uid pedestrianComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_PEDESTRIANCOMPLIANCEID)));
    uid handicappedComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));
    uid reservationRuleId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_RESERVATIONRULEID)));
    
    synthese::env::Line* line = new synthese::env::Line (id, name, 
							 environment.getAxes ().get (axisId));

    line->setNetwork (environment.getTransportNetworks().get (transportNetworkId));
    line->setShortName (shortName);
    line->setLongName (longName);
    line->setColor (synthese::util::RGBColor (color));
    line->setStyle (style);
    line->setImage (image);
    line->setTimetableName (timetableName);
    line->setDirection (direction);
    line->setWalkingLine (isWalkingLine);
    line->setUseInDepartureBoards (useInDepartureBoards);
    line->setUseInTimetables (useInTimetables);
    line->setUseInRoutePlanning (useInRoutePlanning);
    line->setRollingStockId (rollingStockId);
    line->setFare (environment.getFares ().get (fareId));
    line->setAlarm (environment.getAlarms ().get (alarmId));
    line->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    line->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    line->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    line->setReservationRule (environment.getReservationRules ().get (reservationRuleId));

    environment.getLines ().add (line);
}



void 
LineTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    synthese::env::Line* line = environment.getLines ().get (id);

    std::string name (
	rows.getColumn (rowIndex, LINES_TABLE_COL_NAME));
    std::string shortName (
	rows.getColumn (rowIndex, LINES_TABLE_COL_SHORTNAME));
    std::string longName (
	rows.getColumn (rowIndex, LINES_TABLE_COL_LONGNAME));
    std::string color (
	rows.getColumn (rowIndex, LINES_TABLE_COL_COLOR));
    std::string style (
	rows.getColumn (rowIndex, LINES_TABLE_COL_STYLE));
    std::string image (
	rows.getColumn (rowIndex, LINES_TABLE_COL_IMAGE));
    std::string timetableName (
	rows.getColumn (rowIndex, LINES_TABLE_COL_TIMETABLENAME));
    std::string direction (
	rows.getColumn (rowIndex, LINES_TABLE_COL_DIRECTION));

    bool isWalkingLine (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_ISWALKINGLINE)));
    bool useInDepartureBoards (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_USEINDEPARTUREBOARDS)));
    bool useInTimetables (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_USEINTIMETABLES)));
    bool useInRoutePlanning (Conversion::ToBool (rows.getColumn (rowIndex, LINES_TABLE_COL_USEINROUTEPLANNING)));

    uid rollingStockId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_ROLLINGSTOCKID)));
    uid fareId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_FAREID)));
    uid alarmId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_ALARMID)));
    uid bikeComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_BIKECOMPLIANCEID)));
    uid pedestrianComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_PEDESTRIANCOMPLIANCEID)));
    uid handicappedComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_HANDICAPPEDCOMPLIANCEID)));
    uid reservationRuleId (Conversion::ToLongLong (rows.getColumn (rowIndex, LINES_TABLE_COL_RESERVATIONRULEID)));
    
    line->setName (name);
    line->setShortName (shortName);
    line->setLongName (longName);
    line->setColor (synthese::util::RGBColor (color));
    line->setStyle (style);
    line->setImage (image);
    line->setTimetableName (timetableName);
    line->setDirection (direction);
    line->setWalkingLine (isWalkingLine);
    line->setUseInDepartureBoards (useInDepartureBoards);
    line->setUseInTimetables (useInTimetables);
    line->setUseInRoutePlanning (useInRoutePlanning);
    line->setRollingStockId (rollingStockId);
    line->setFare (environment.getFares ().get (fareId));
    line->setAlarm (environment.getAlarms ().get (alarmId));
    line->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    line->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    line->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    line->setReservationRule (environment.getReservationRules ().get (reservationRuleId));

    
}



void 
LineTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getLines ().remove (id);
}













}

}
