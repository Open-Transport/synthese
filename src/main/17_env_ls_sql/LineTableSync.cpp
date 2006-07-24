#include "LineTableSync.h"

#include "01_util/RGBColor.h"
#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Line.h"

#include <sqlite/sqlite3.h>
#include <boost/logic/tribool.hpp>


using boost::logic::tribool;

using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



LineTableSync::LineTableSync (Environment::Registry& environments)
: ComponentTableSync (LINES_TABLE_NAME, environments)
{
    addTableColumn (LINES_TABLE_COL_TRANSPORTNETWORKID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_AXISID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_NAME, "TEXT");
    addTableColumn (LINES_TABLE_COL_SHORTNAME, "TEXT");
    addTableColumn (LINES_TABLE_COL_LONGNAME, "TEXT");
    addTableColumn (LINES_TABLE_COL_COLOR, "TEXT");

    addTableColumn (LINES_TABLE_COL_STYLE, "TEXT");
    addTableColumn (LINES_TABLE_COL_IMAGE, "TEXT");
    addTableColumn (LINES_TABLE_COL_TIMETABLENAME, "TEXT");
    addTableColumn (LINES_TABLE_COL_DIRECTION, "TEXT");
    addTableColumn (LINES_TABLE_COL_ISWALKINGLINE, "BOOLEAN");
    addTableColumn (LINES_TABLE_COL_USEINDEPARTUREBOARDS, "BOOLEAN");
    addTableColumn (LINES_TABLE_COL_USEINTIMETABLES, "BOOLEAN");
    addTableColumn (LINES_TABLE_COL_USEINROUTEPLANNING, "BOOLEAN");

    addTableColumn (LINES_TABLE_COL_ROLLINGSTOCKID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_FAREID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_ALARMID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_BIKECOMPLIANCEID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_HANDICAPPEDCOMPLIANCEID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_PEDESTRIANCOMPLIANCEID, "INTEGER");
    addTableColumn (LINES_TABLE_COL_RESERVATIONRULEID, "INTEGER");

}


LineTableSync::~LineTableSync ()
{
}


void 
LineTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    
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
    line->setRollingStock (environment.getRollingStocks ().get (rollingStockId));
    line->setFare (environment.getFares ().get (fareId));
    line->setAlarm (environment.getAlarms ().get (alarmId));
    line->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    line->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    line->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    line->setReservationRule (environment.getReservationRules ().get (reservationRuleId));

    environment.getLines ().add (line, false);
}



void 
LineTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
}



void 
LineTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
}













}

}
