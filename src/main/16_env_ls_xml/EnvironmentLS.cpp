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
#include "LineStopLS.h"


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
const std::string EnvironmentLS::ENVIRONMENT_LINESTOPS_TAG ("lineStops");


synthese::env::Environment* 
EnvironmentLS::Load (XMLNode& node)
{
    assert (ENVIRONMENT_TAG == node.getName ());

    int id (su::Conversion::ToInt (
		node.getAttribute (ENVIRONMENT_ID_ATTR.c_str())));
    synthese::env::Environment* env = new synthese::env::Environment (id);

    XMLNode citiesNode = node.getChildNode(ENVIRONMENT_CITIES_TAG.c_str(), 0);
    int nbCities = citiesNode.nChildNode(CityLS::CITY_TAG.c_str());
    for (int i=0; i<nbCities; ++i) 
    {
	XMLNode cityNode = node.getChildNode (CityLS::CITY_TAG.c_str(), i);
	env->getCities ().add (CityLS::Load (cityNode, *env));
    }

    XMLNode axesNode = node.getChildNode(ENVIRONMENT_AXES_TAG.c_str(), 0);
    int nbAxes = axesNode.nChildNode(AxisLS::AXIS_TAG.c_str());
    for (int i=0; i<nbAxes; ++i) 
    {
	XMLNode axisNode = node.getChildNode (AxisLS::AXIS_TAG.c_str(), i);
	env->getAxes ().add (AxisLS::Load (axisNode, *env));
    }
    
    XMLNode linesNode = node.getChildNode(ENVIRONMENT_LINES_TAG.c_str(), 0);
    int nbLines = linesNode.nChildNode(LineLS::LINE_TAG.c_str());
    for (int i=0; i<nbLines; ++i) 
    {
	XMLNode lineNode = node.getChildNode (LineLS::LINE_TAG.c_str(), i);
	env->getLines ().add (LineLS::Load (lineNode, *env));
    }
    
    XMLNode logicalStopsNode = node.getChildNode(ENVIRONMENT_LOGICALSTOPS_TAG.c_str(), 0);
    int nbLogicalStops = logicalStopsNode.nChildNode(LogicalStopLS::LOGICALSTOP_TAG.c_str());
    for (int i=0; i<nbLogicalStops; ++i) 
    {
	XMLNode logicalStopNode = node.getChildNode (LogicalStopLS::LOGICALSTOP_TAG.c_str(), i);
	env->getLogicalStops ().add (LogicalStopLS::Load (logicalStopNode, *env));
    }
    
    XMLNode physicalStopsNode = node.getChildNode(ENVIRONMENT_PHYSICALSTOPS_TAG.c_str(), 0);
    int nbPhysicalStops = physicalStopsNode.nChildNode(PhysicalStopLS::PHYSICALSTOP_TAG.c_str());
    for (int i=0; i<nbPhysicalStops; ++i) 
    {
	XMLNode physicalStopNode = node.getChildNode (PhysicalStopLS::PHYSICALSTOP_TAG.c_str(), i);
	env->getPhysicalStops ().add (PhysicalStopLS::Load (physicalStopNode, *env));
    }
    
    XMLNode lineStopsNode = node.getChildNode(ENVIRONMENT_LINESTOPS_TAG.c_str(), 0);
    int nbLineStops = lineStopsNode.nChildNode(LineStopLS::LINESTOP_TAG.c_str());
    for (int i=0; i<nbLineStops; ++i) 
    {
	XMLNode lineStopNode = node.getChildNode (LineStopLS::LINESTOP_TAG.c_str(), i);
	env->getLineStops ().add (LineStopLS::Load (lineStopNode, *env));
    }

    return env;
}




XMLNode* 
EnvironmentLS::Save (const synthese::env::Environment* environment)
{
    // ...
}






}
}

