#include "RoadLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/Road.h"
#include "15_env/City.h"


using namespace synthese::util::XmlToolkit;

using synthese::env::Road;
using synthese::env::City;


namespace synthese
{
namespace envlsxml
{

    const std::string RoadLS::ROAD_TAG ("road");
    const std::string RoadLS::ROAD_ID_ATTR ("id");
    const std::string RoadLS::ROAD_NAME_ATTR ("name");
    const std::string RoadLS::ROAD_CITYID_ATTR ("cityId");

    const std::string RoadLS::ROAD_TYPE_ATTR ("type");
    const std::string RoadLS::ROAD_TYPE_ATTR_UNKNOWN ("unknown");
    const std::string RoadLS::ROAD_TYPE_ATTR_MOTORWAY ("motorway");
    const std::string RoadLS::ROAD_TYPE_ATTR_MEDIANSTRIPPEDROAD ("medianStrippedRoad");
    const std::string RoadLS::ROAD_TYPE_ATTR_PRINCIPLEAXIS ("principleAxis");
    const std::string RoadLS::ROAD_TYPE_ATTR_SECONDARYAXIS ("secondaryAxis");
    const std::string RoadLS::ROAD_TYPE_ATTR_BRIDGE ("bridge");
    const std::string RoadLS::ROAD_TYPE_ATTR_STREET ("street");
    const std::string RoadLS::ROAD_TYPE_ATTR_PEDESTRIANSTREET ("pedestrianStreet");
    const std::string RoadLS::ROAD_TYPE_ATTR_ACCESSROAD ("accessRoad");
    const std::string RoadLS::ROAD_TYPE_ATTR_PRIVATEWAY ("privateWay");
    const std::string RoadLS::ROAD_TYPE_ATTR_PEDESTRIANPATH ("pedestrianPath");
    const std::string RoadLS::ROAD_TYPE_ATTR_TUNNEL ("tunnel");
    const std::string RoadLS::ROAD_TYPE_ATTR_HIGHWAY ("highway");

    const std::string RoadLS::ROAD_FAREID_ATTR ("fareId");
    const std::string RoadLS::ROAD_ALARMID_ATTR ("alarmId");
    const std::string RoadLS::ROAD_BIKECOMPLIANCEID_ATTR ("bikeComplianceId");
    const std::string RoadLS::ROAD_HANDICAPPEDCOMPLIANCEID_ATTR ("handicappedComplianceId");
    const std::string RoadLS::ROAD_PEDESTRIANCOMPLIANCEID_ATTR ("pedestrianComplianceId");
    const std::string RoadLS::ROAD_RESERVATIONRULEID_ATTR ("reservationRuleId");
    

void 
RoadLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (ROAD_TAG == node.getName ());
    uid id (GetLongLongAttr (node, ROAD_ID_ATTR));

    if (environment.getRoads ().contains (id)) return;

    std::string name (GetStringAttr (node, ROAD_NAME_ATTR));
    uid cityId (GetLongLongAttr (node, ROAD_CITYID_ATTR));

    std::string roadTypeStr (GetStringAttr (node, ROAD_TYPE_ATTR, ROAD_TYPE_ATTR_UNKNOWN));
    Road::RoadType roadType (Road::ROAD_TYPE_UNKNOWN);
    
    if (roadTypeStr == ROAD_TYPE_ATTR_UNKNOWN) roadType = Road::ROAD_TYPE_UNKNOWN;
    else if (roadTypeStr == ROAD_TYPE_ATTR_MOTORWAY) roadType = Road::ROAD_TYPE_MOTORWAY;
    else if (roadTypeStr == ROAD_TYPE_ATTR_MEDIANSTRIPPEDROAD) roadType = Road::ROAD_TYPE_MEDIANSTRIPPEDROAD;
    else if (roadTypeStr == ROAD_TYPE_ATTR_PRINCIPLEAXIS) roadType = Road::ROAD_TYPE_PRINCIPLEAXIS;
    else if (roadTypeStr == ROAD_TYPE_ATTR_SECONDARYAXIS) roadType = Road::ROAD_TYPE_SECONDARYAXIS;
    else if (roadTypeStr == ROAD_TYPE_ATTR_BRIDGE) roadType = Road::ROAD_TYPE_BRIDGE;
    else if (roadTypeStr == ROAD_TYPE_ATTR_STREET) roadType = Road::ROAD_TYPE_STREET;
    else if (roadTypeStr == ROAD_TYPE_ATTR_PEDESTRIANSTREET) roadType = Road::ROAD_TYPE_PEDESTRIANSTREET;
    else if (roadTypeStr == ROAD_TYPE_ATTR_ACCESSROAD) roadType = Road::ROAD_TYPE_ACCESSROAD;
    else if (roadTypeStr == ROAD_TYPE_ATTR_PRIVATEWAY) roadType = Road::ROAD_TYPE_PRIVATEWAY;
    else if (roadTypeStr == ROAD_TYPE_ATTR_PEDESTRIANPATH) roadType = Road::ROAD_TYPE_PEDESTRIANPATH;
    else if (roadTypeStr == ROAD_TYPE_ATTR_TUNNEL) roadType = Road::ROAD_TYPE_TUNNEL;
    else if (roadTypeStr == ROAD_TYPE_ATTR_HIGHWAY) roadType = Road::ROAD_TYPE_HIGHWAY;
    
    City* city = environment.getCities ().get (cityId);
    
    Road* road = new synthese::env::Road (id, name, 
					  city, 
					  roadType);

    if (HasAttr (node, ROAD_FAREID_ATTR)) 
    {
	uid fareId (GetLongLongAttr (node, ROAD_FAREID_ATTR));
	road->setFare (environment.getFares ().get (fareId));
    }

    if (HasAttr (node, ROAD_ALARMID_ATTR)) 
    {
	uid alarmId (GetLongLongAttr (node, ROAD_ALARMID_ATTR));
//	road->setAlarm (environment.getAlarms ().get (alarmId));
    }

    if (HasAttr (node, ROAD_BIKECOMPLIANCEID_ATTR)) 
    {
	uid bikeComplianceId (GetLongLongAttr (node, ROAD_BIKECOMPLIANCEID_ATTR));
	road->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    }

    if (HasAttr (node, ROAD_HANDICAPPEDCOMPLIANCEID_ATTR)) 
    {
	uid handicappedComplianceId (GetLongLongAttr (node, ROAD_HANDICAPPEDCOMPLIANCEID_ATTR));
	road->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    }

    if (HasAttr (node, ROAD_PEDESTRIANCOMPLIANCEID_ATTR)) 
    {
	uid pedestrianComplianceId (GetLongLongAttr (node, ROAD_PEDESTRIANCOMPLIANCEID_ATTR));
	road->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));
    }
    
    if (HasAttr (node, ROAD_RESERVATIONRULEID_ATTR)) 
    {
	uid reservationRuleId (GetLongLongAttr (node, ROAD_RESERVATIONRULEID_ATTR));
	road->setReservationRule (environment.getReservationRules ().get (reservationRuleId)); 
    }

    
    environment.getRoads ().add (road);
    city->getRoadsMatcher ().add (road->getName (), road);

}




XMLNode* 
RoadLS::Save (const synthese::env::Road* road)
{
    // ...
    return 0;
}






}
}


