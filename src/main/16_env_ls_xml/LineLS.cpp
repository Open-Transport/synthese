#include "LineLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/RGBColor.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/Line.h"

#include "LineStopLS.h"


using namespace synthese::util::XmlToolkit;




namespace synthese
{
namespace envlsxml
{




const std::string LineLS::LINE_TAG ("line");
const std::string LineLS::LINE_ID_ATTR ("id");
const std::string LineLS::LINE_TRANSPORTNETWORKID_ATTR ("transportNetworkId");
const std::string LineLS::LINE_AXISID_ATTR ("axisId");
const std::string LineLS::LINE_NAME_ATTR ("name");
const std::string LineLS::LINE_SHORTNAME_ATTR ("shortName");
const std::string LineLS::LINE_LONGNAME_ATTR ("longName");
const std::string LineLS::LINE_COLOR_ATTR ("color");
const std::string LineLS::LINE_STYLE_ATTR ("style");
const std::string LineLS::LINE_IMAGE_ATTR ("image");
const std::string LineLS::LINE_TIMETABLENAME_ATTR ("timetableName");
const std::string LineLS::LINE_DIRECTION_ATTR ("direction");
const std::string LineLS::LINE_ISWALKINGLINE_ATTR ("isWalkingLine");
const std::string LineLS::LINE_USEINDEPARTUREBOARDS_ATTR ("useInDepartureBoards");
const std::string LineLS::LINE_USEINTIMETABLES_ATTR ("useInTimetables");
const std::string LineLS::LINE_USEINROUTEPLANNING_ATTR ("useInRoutePlanning");
const std::string LineLS::LINE_ROLLINGSTOCKID_ATTR ("rollingStockId");
const std::string LineLS::LINE_FAREID_ATTR ("fareId");
const std::string LineLS::LINE_ALARMID_ATTR ("alarmId");
const std::string LineLS::LINE_BIKECOMPLIANCEID_ATTR ("bikeComplianceId");
const std::string LineLS::LINE_HANDICAPPEDCOMPLIANCEID_ATTR ("handicappedComplianceId");
const std::string LineLS::LINE_PEDESTRIANCOMPLIANCEID_ATTR ("pedestrianComplianceId");
const std::string LineLS::LINE_RESERVATIONRULEID_ATTR ("reservationRuleId");





void
LineLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    uid id (GetLongLongAttr (node, LINE_ID_ATTR));

    if (environment.getLines ().contains (id)) return;

    std::string name (GetStringAttr (node, LINE_NAME_ATTR));
    uid axisId (GetLongLongAttr (node, LINE_AXISID_ATTR));

    synthese::env::Line* line = new synthese::env::Line (id, name, 
							 environment.getAxes ().get (axisId));

    uid transportNetworkId (GetLongLongAttr (node, LINE_TRANSPORTNETWORKID_ATTR));
    line->setNetwork (environment.getTransportNetworks().get (transportNetworkId));

    std::string shortName (GetStringAttr (node, LINE_SHORTNAME_ATTR, name));
    line->setShortName (shortName);

    std::string longName (GetStringAttr (node, LINE_LONGNAME_ATTR, name));
    line->setLongName (longName);

    std::string color (GetStringAttr (node, LINE_COLOR_ATTR));
    line->setColor (synthese::util::RGBColor (color));

    std::string style (GetStringAttr (node, LINE_STYLE_ATTR, ""));
    line->setStyle (style);

    std::string image (GetStringAttr (node, LINE_IMAGE_ATTR, ""));
    line->setImage (image);

    std::string timetableName (GetStringAttr (node, LINE_TIMETABLENAME_ATTR, name));
    line->setTimetableName (timetableName);

    std::string direction (GetStringAttr (node, LINE_DIRECTION_ATTR, ""));
    line->setDirection (direction);

    bool isWalkingLine (GetBoolAttr (node, LINE_ISWALKINGLINE_ATTR, false));
    line->setWalkingLine (isWalkingLine);

    bool useInDepartureBoards (GetBoolAttr (node, LINE_USEINDEPARTUREBOARDS_ATTR, true));
    line->setUseInDepartureBoards (useInDepartureBoards);

    bool useInTimetables (GetBoolAttr (node, LINE_USEINTIMETABLES_ATTR, true));
    line->setUseInTimetables (useInTimetables);

    bool useInRoutePlanning (GetBoolAttr (node, LINE_USEINROUTEPLANNING_ATTR, true));
    line->setUseInRoutePlanning (useInRoutePlanning);

    if (HasAttr (node, LINE_ROLLINGSTOCKID_ATTR))
    {
	uid rollingStockId (GetLongLongAttr (node, LINE_ROLLINGSTOCKID_ATTR));
	line->setRollingStockId (rollingStockId);
    }
    
    if (HasAttr (node, LINE_FAREID_ATTR))
    {
	uid fareId (GetLongLongAttr (node, LINE_FAREID_ATTR));
	line->setFare (environment.getFares ().get (fareId));
    }

    if (HasAttr (node, LINE_ALARMID_ATTR))
    {
	uid alarmId (GetLongLongAttr (node, LINE_ALARMID_ATTR));
//	line->setAlarm (environment.getAlarms ().get (alarmId));
    }

    uid bikeComplianceId (GetLongLongAttr (node, LINE_BIKECOMPLIANCEID_ATTR));
    line->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));

    uid pedestrianComplianceId (GetLongLongAttr (node, LINE_PEDESTRIANCOMPLIANCEID_ATTR));
    line->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));

    uid handicappedComplianceId (GetLongLongAttr (node, LINE_HANDICAPPEDCOMPLIANCEID_ATTR));
    line->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));

    uid reservationRuleId (GetLongLongAttr (node, LINE_RESERVATIONRULEID_ATTR));
    line->setReservationRule (environment.getReservationRules ().get (reservationRuleId));

    environment.getLines ().add (line);
}




XMLNode* 
LineLS::Save (const synthese::env::Line* line)
{
    // ...
    return 0;

}






}
}


