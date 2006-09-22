#include "EnvironmentLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/Line.h"

#include "AxisLS.h"
#include "CityLS.h"
#include "LineLS.h"
#include "LineStopLS.h"
#include "ConnectionPlaceLS.h"
#include "PhysicalStopLS.h"
#include "RoadLS.h"
#include "AddressLS.h"
#include "RoadChunkLS.h"
#include "BikeComplianceLS.h"
#include "HandicappedComplianceLS.h"
#include "PedestrianComplianceLS.h"
#include "ReservationRuleLS.h"
#include "FareLS.h"
#include "ContinuousServiceLS.h"
#include "ScheduledServiceLS.h"
#include "AlarmLS.h"
#include "TransportNetworkLS.h"


using namespace synthese::util::XmlToolkit;



namespace synthese
{
namespace envlsxml
{

const std::string EnvironmentLS::ENVIRONMENT_TAG ("environment");
const std::string EnvironmentLS::ENVIRONMENT_ID_ATTR ("id");
const std::string EnvironmentLS::ENVIRONMENT_CITIES_TAG ("cities");
const std::string EnvironmentLS::ENVIRONMENT_TRANSPORTNETWORKS_TAG ("transportNetworks");
const std::string EnvironmentLS::ENVIRONMENT_AXES_TAG ("axes");
const std::string EnvironmentLS::ENVIRONMENT_LINES_TAG ("lines");
const std::string EnvironmentLS::ENVIRONMENT_LINESTOPS_TAG ("lineStops");
const std::string EnvironmentLS::ENVIRONMENT_CONNECTIONPLACES_TAG ("connectionPlaces");
const std::string EnvironmentLS::ENVIRONMENT_PHYSICALSTOPS_TAG ("physicalStops");
const std::string EnvironmentLS::ENVIRONMENT_ROADS_TAG ("roads");
const std::string EnvironmentLS::ENVIRONMENT_ADDRESSES_TAG ("addresses");
const std::string EnvironmentLS::ENVIRONMENT_ROADCHUNKS_TAG ("roadChunks");
const std::string EnvironmentLS::ENVIRONMENT_BIKECOMPLIANCES_TAG ("bikeCompliances");
const std::string EnvironmentLS::ENVIRONMENT_HANDICAPPEDCOMPLIANCES_TAG ("handicappedCompliances");
const std::string EnvironmentLS::ENVIRONMENT_PEDESTRIANCOMPLIANCES_TAG ("pedestrianCompliances");
const std::string EnvironmentLS::ENVIRONMENT_RESERVATIONRULES_TAG ("reservationRules");
const std::string EnvironmentLS::ENVIRONMENT_FARES_TAG ("fares");
const std::string EnvironmentLS::ENVIRONMENT_CONTINUOUSSERVICES_TAG ("continuousServices");
const std::string EnvironmentLS::ENVIRONMENT_SCHEDULEDSERVICES_TAG ("scheduledServices");
const std::string EnvironmentLS::ENVIRONMENT_ALARMS_TAG ("alarms");


synthese::env::Environment* 
EnvironmentLS::Load (XMLNode& node)
{
    uid id (GetLongLongAttr (node, ENVIRONMENT_ID_ATTR));

    synthese::env::Environment* env = new synthese::env::Environment (id);

    XMLNode citiesNode = GetChildNode (node, ENVIRONMENT_CITIES_TAG, 0);
    int nbCities = GetChildNodeCount (citiesNode, CityLS::CITY_TAG);
    for (int i=0; i<nbCities; ++i) 
    {
	XMLNode cityNode = GetChildNode (citiesNode, CityLS::CITY_TAG, i);
	CityLS::Load (cityNode, *env);
    }


    XMLNode transportNetworksNode = GetChildNode (node, ENVIRONMENT_TRANSPORTNETWORKS_TAG, 0);
    int nbTransportNetworks = GetChildNodeCount (transportNetworksNode, TransportNetworkLS::TRANSPORTNETWORK_TAG);
    for (int i=0; i<nbTransportNetworks; ++i) 
    {
	XMLNode transportNetworkNode = GetChildNode (transportNetworksNode, TransportNetworkLS::TRANSPORTNETWORK_TAG, i);
	TransportNetworkLS::Load (transportNetworkNode, *env);
    }

    XMLNode bikeCompliancesNode = GetChildNode (node, ENVIRONMENT_BIKECOMPLIANCES_TAG, 0);
    int nbBikeCompliances = GetChildNodeCount (bikeCompliancesNode, BikeComplianceLS::BIKECOMPLIANCE_TAG);
    for (int i=0; i<nbBikeCompliances; ++i) 
    {
	XMLNode bikeComplianceNode = GetChildNode (bikeCompliancesNode, BikeComplianceLS::BIKECOMPLIANCE_TAG, i);
	BikeComplianceLS::Load (bikeComplianceNode, *env);
    }

    XMLNode handicappedCompliancesNode = GetChildNode (node, ENVIRONMENT_HANDICAPPEDCOMPLIANCES_TAG, 0);
    int nbHandicappedCompliances = GetChildNodeCount (handicappedCompliancesNode, HandicappedComplianceLS::HANDICAPPEDCOMPLIANCE_TAG);
    for (int i=0; i<nbHandicappedCompliances; ++i) 
    {
	XMLNode handicappedComplianceNode = GetChildNode (handicappedCompliancesNode, HandicappedComplianceLS::HANDICAPPEDCOMPLIANCE_TAG, i);
	HandicappedComplianceLS::Load (handicappedComplianceNode, *env);
    }

    XMLNode pedestrianCompliancesNode = GetChildNode (node, ENVIRONMENT_PEDESTRIANCOMPLIANCES_TAG, 0);
    int nbPedestrianCompliances = GetChildNodeCount (pedestrianCompliancesNode, PedestrianComplianceLS::PEDESTRIANCOMPLIANCE_TAG);
    for (int i=0; i<nbPedestrianCompliances; ++i) 
    {
	XMLNode pedestrianComplianceNode = GetChildNode (pedestrianCompliancesNode, PedestrianComplianceLS::PEDESTRIANCOMPLIANCE_TAG, i);
	PedestrianComplianceLS::Load (pedestrianComplianceNode, *env);
    }


    XMLNode reservationRulesNode = GetChildNode (node, ENVIRONMENT_RESERVATIONRULES_TAG, 0);
    int nbReservationRules = GetChildNodeCount (reservationRulesNode, ReservationRuleLS::RESERVATIONRULE_TAG);
    for (int i=0; i<nbReservationRules; ++i) 
    {
	XMLNode reservationRuleNode = GetChildNode (reservationRulesNode, ReservationRuleLS::RESERVATIONRULE_TAG, i);
	ReservationRuleLS::Load (reservationRuleNode, *env);
    }


    XMLNode faresNode = GetChildNode (node, ENVIRONMENT_FARES_TAG, 0);
    int nbFares = GetChildNodeCount (faresNode, FareLS::FARE_TAG);
    for (int i=0; i<nbFares; ++i) 
    {
	XMLNode fareNode = GetChildNode (faresNode, FareLS::FARE_TAG, i);
	FareLS::Load (fareNode, *env);
    }

    XMLNode alarmsNode = GetChildNode (node, ENVIRONMENT_ALARMS_TAG, 0);
    int nbAlarms = GetChildNodeCount (alarmsNode, AlarmLS::ALARM_TAG);
    for (int i=0; i<nbAlarms; ++i) 
    {
	XMLNode alarmNode = GetChildNode (alarmsNode, AlarmLS::ALARM_TAG, i);
	AlarmLS::Load (alarmNode, *env);
    }


    XMLNode axesNode = GetChildNode (node, ENVIRONMENT_AXES_TAG, 0);
    int nbAxes = GetChildNodeCount (axesNode, AxisLS::AXIS_TAG);
    for (int i=0; i<nbAxes; ++i) 
    {
	XMLNode axisNode = GetChildNode (axesNode, AxisLS::AXIS_TAG, i);
	AxisLS::Load (axisNode, *env);
    }
    
    XMLNode connectionPlacesNode = GetChildNode (node, ENVIRONMENT_CONNECTIONPLACES_TAG, 0);
    int nbConnectionPlaces = GetChildNodeCount (connectionPlacesNode, ConnectionPlaceLS::CONNECTIONPLACE_TAG);
    for (int i=0; i<nbConnectionPlaces; ++i) 
    {
	XMLNode connectionPlaceNode = GetChildNode (connectionPlacesNode, ConnectionPlaceLS::CONNECTIONPLACE_TAG, i);
	ConnectionPlaceLS::Load (connectionPlaceNode, *env);
    }
    
    XMLNode physicalStopsNode = GetChildNode (node, ENVIRONMENT_PHYSICALSTOPS_TAG, 0);
    int nbPhysicalStops = GetChildNodeCount (physicalStopsNode, PhysicalStopLS::PHYSICALSTOP_TAG);
    for (int i=0; i<nbPhysicalStops; ++i) 
    {
	XMLNode physicalStopNode = GetChildNode (physicalStopsNode, PhysicalStopLS::PHYSICALSTOP_TAG, i);
	PhysicalStopLS::Load (physicalStopNode, *env);
    }
    
    XMLNode linesNode = GetChildNode (node, ENVIRONMENT_LINES_TAG, 0);
    int nbLines = GetChildNodeCount (linesNode, LineLS::LINE_TAG);
    for (int i=0; i<nbLines; ++i) 
    {
	XMLNode lineNode = GetChildNode (linesNode, LineLS::LINE_TAG, i);
	LineLS::Load (lineNode, *env);
    }


    XMLNode lineStopsNode = GetChildNode (node, ENVIRONMENT_LINESTOPS_TAG, 0);
    int nbLineStops = GetChildNodeCount (lineStopsNode, LineStopLS::LINESTOP_TAG);
    for (int i=0; i<nbLineStops; ++i) 
    {
	XMLNode lineStopNode = GetChildNode (lineStopsNode, LineStopLS::LINESTOP_TAG, i);
	LineStopLS::Load (lineStopNode, *env);
    }

    XMLNode roadsNode = GetChildNode (node, ENVIRONMENT_ROADS_TAG, 0);
    int nbRoads = GetChildNodeCount (roadsNode, RoadLS::ROAD_TAG);
    for (int i=0; i<nbRoads; ++i) 
    {
	XMLNode roadNode = GetChildNode (roadsNode, RoadLS::ROAD_TAG, i);
	RoadLS::Load (roadNode, *env);
    }


    XMLNode addressesNode = GetChildNode (node, ENVIRONMENT_ADDRESSES_TAG, 0);
    int nbAddresses = GetChildNodeCount (addressesNode, AddressLS::ADDRESS_TAG);
    for (int i=0; i<nbAddresses; ++i) 
    {
	XMLNode addressNode = GetChildNode (addressesNode, AddressLS::ADDRESS_TAG, i);
	AddressLS::Load (addressNode, *env);
    }


    XMLNode roadChunksNode = GetChildNode (node, ENVIRONMENT_ROADCHUNKS_TAG, 0);
    int nbRoadChunks = GetChildNodeCount (roadChunksNode, RoadChunkLS::ROADCHUNK_TAG);
    for (int i=0; i<nbRoadChunks; ++i) 
    {
	XMLNode roadChunkNode = GetChildNode (roadChunksNode, RoadChunkLS::ROADCHUNK_TAG, i);
	RoadChunkLS::Load (roadChunkNode, *env);
    }



    XMLNode continuousServicesNode = GetChildNode (node, ENVIRONMENT_CONTINUOUSSERVICES_TAG, 0);
    int nbContinuousServices = GetChildNodeCount (continuousServicesNode, ContinuousServiceLS::CONTINUOUSSERVICE_TAG);
    for (int i=0; i<nbContinuousServices; ++i) 
    {
	XMLNode continuousServiceNode = GetChildNode (continuousServicesNode, ContinuousServiceLS::CONTINUOUSSERVICE_TAG, i);
	ContinuousServiceLS::Load (continuousServiceNode, *env);
    }

    XMLNode scheduledServicesNode = GetChildNode (node, ENVIRONMENT_SCHEDULEDSERVICES_TAG, 0);
    int nbScheduledServices = GetChildNodeCount (scheduledServicesNode, ScheduledServiceLS::SCHEDULEDSERVICE_TAG);
    for (int i=0; i<nbScheduledServices; ++i) 
    {
	XMLNode scheduledServiceNode = GetChildNode (scheduledServicesNode, ScheduledServiceLS::SCHEDULEDSERVICE_TAG, i);
	ScheduledServiceLS::Load (scheduledServiceNode, *env);
    }


    return env;
}




XMLNode* 
EnvironmentLS::Save (const synthese::env::Environment* environment)
{
    // ...
    return 0;

}






}
}

