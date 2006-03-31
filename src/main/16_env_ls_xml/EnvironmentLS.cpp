#include "EnvironmentLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"

#include "AxisLS.h"
#include "CityLS.h"
#include "LineLS.h"
#include "LogicalStopLS.h"
#include "PhysicalStopLS.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string EnvironmentLS::ENVIRONMENT_TAG ("environment");
const std::string EnvironmentLS::ENVIRONMENT_ID_ATTR ("id");
const std::string EnvironmentLS::ENVIRONMENT_CITIES_TAG ("cities");
const std::string EnvironmentLS::ENVIRONMENT_AXES_TAG ("axes");
const std::string EnvironmentLS::ENVIRONMENT_LINES_TAG ("lines");
const std::string EnvironmentLS::ENVIRONMENT_LOGICALSTOPS_TAG ("logicalStops");
const std::string EnvironmentLS::ENVIRONMENT_PHYSICALSTOPS_TAG ("physicalStops");


synthese::env::Environment* 
EnvironmentLS::Load (XMLNode& node)
{
    int id (su::Conversion::ToInt (
		node.getAttribute (ENVIRONMENT_ID_ATTR.c_str())));
    synthese::env::Environment* env = new synthese::env::Environment (id);

    XMLNode citiesNode = node.getChildNode(ENVIRONMENT_CITIES_TAG.c_str(), 0);
    int nbCities = citiesNode.nChildNode(CityLS::CITY_TAG.c_str());
    for (int i=0; i<nbCities; ++i) 
    {
	XMLNode cityNode = citiesNode.getChildNode (CityLS::CITY_TAG.c_str(), i);
	env->getCities ().add (CityLS::Load (cityNode, *env));
    }

    XMLNode axesNode = node.getChildNode(ENVIRONMENT_AXES_TAG.c_str(), 0);
    int nbAxes = axesNode.nChildNode(AxisLS::AXIS_TAG.c_str());
    for (int i=0; i<nbAxes; ++i) 
    {
	XMLNode axisNode = axesNode.getChildNode (AxisLS::AXIS_TAG.c_str(), i);
	env->getAxes ().add (AxisLS::Load (axisNode, *env));
    }
    
    XMLNode logicalStopsNode = node.getChildNode(ENVIRONMENT_LOGICALSTOPS_TAG.c_str(), 0);
    int nbLogicalStops = logicalStopsNode.nChildNode(LogicalStopLS::LOGICALSTOP_TAG.c_str());
    for (int i=0; i<nbLogicalStops; ++i) 
    {
	XMLNode logicalStopNode = logicalStopsNode.getChildNode (LogicalStopLS::LOGICALSTOP_TAG.c_str(), i);
	env->getLogicalStops ().add (LogicalStopLS::Load (logicalStopNode, *env));
    }
    
    XMLNode physicalStopsNode = node.getChildNode(ENVIRONMENT_PHYSICALSTOPS_TAG.c_str(), 0);
    int nbPhysicalStops = physicalStopsNode.nChildNode(PhysicalStopLS::PHYSICALSTOP_TAG.c_str());
    for (int i=0; i<nbPhysicalStops; ++i) 
    {
	XMLNode physicalStopNode = physicalStopsNode.getChildNode (PhysicalStopLS::PHYSICALSTOP_TAG.c_str(), i);
	env->getPhysicalStops ().add (PhysicalStopLS::Load (physicalStopNode, *env));
    }
    
    XMLNode linesNode = node.getChildNode(ENVIRONMENT_LINES_TAG.c_str(), 0);
    int nbLines = linesNode.nChildNode(LineLS::LINE_TAG.c_str());
    for (int i=0; i<nbLines; ++i) 
    {
	XMLNode lineNode = linesNode.getChildNode (LineLS::LINE_TAG.c_str(), i);
	env->getLines ().add (LineLS::Load (lineNode, *env));
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

