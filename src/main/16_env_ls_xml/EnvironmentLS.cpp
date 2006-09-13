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


using namespace synthese::util::XmlToolkit;



namespace synthese
{
namespace envlsxml
{

const std::string EnvironmentLS::ENVIRONMENT_TAG ("environment");
const std::string EnvironmentLS::ENVIRONMENT_ID_ATTR ("id");
const std::string EnvironmentLS::ENVIRONMENT_CITIES_TAG ("cities");
const std::string EnvironmentLS::ENVIRONMENT_AXES_TAG ("axes");
const std::string EnvironmentLS::ENVIRONMENT_LINES_TAG ("lines");
const std::string EnvironmentLS::ENVIRONMENT_LINESTOPS_TAG ("lineStops");
const std::string EnvironmentLS::ENVIRONMENT_CONNECTIONPLACES_TAG ("connectionPlaces");
const std::string EnvironmentLS::ENVIRONMENT_PHYSICALSTOPS_TAG ("physicalStops");
const std::string EnvironmentLS::ENVIRONMENT_ROADS_TAG ("roads");
const std::string EnvironmentLS::ENVIRONMENT_ADDRESSES_TAG ("addresses");
const std::string EnvironmentLS::ENVIRONMENT_ROADCHUNKS_TAG ("roadChunks");


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

