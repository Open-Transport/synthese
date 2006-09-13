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


synthese::env::Environment* 
EnvironmentLS::Load (XMLNode& node)
{
    uid id (GetLongLongAttr (node, ENVIRONMENT_ID_ATTR));

    // TODO : le add doit etre fait au niveau de chaque loader
    // en faisant un try catch en cas d'erreur de add dans registre
    // ou plutot guard au debut de chaque load

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

