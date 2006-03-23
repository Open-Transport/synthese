#include "PhysicalStopLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "15_env/PhysicalStop.h"


namespace su = synthese::util;

namespace synthese
{
namespace lsxml
{

const std::string PhysicalStopLS::PHYSICAL_STOP_TAG ("physicalStop");
const std::string PhysicalStopLS::PHYSICAL_STOP_NAME_ATTR ("name");
const std::string PhysicalStopLS::PHYSICAL_STOP_LOGICALSTOPID_ATTR ("logicalStopId");
const std::string PhysicalStopLS::PHYSICAL_STOP_RANKINLOGICALSTOP_ATTR ("rankInLogicalStop");


synthese::env::PhysicalStop* 
PhysicalStopLS::Load (XMLNode& node,
		      const synthese::env::Environment& environment)
{
    assert (PHYSICAL_STOP_TAG == node.getName ());

    std::string name (node.getAttribute (PHYSICAL_STOP_NAME_ATTR.c_str()));
    int logicalStopId (su::Conversion::ToInt (
	      node.getAttribute (PHYSICAL_STOP_LOGICALSTOPID_ATTR.c_str())));
    int rankInLogicalStop (su::Conversion::ToInt (
	      node.getAttribute (PHYSICAL_STOP_RANKINLOGICALSTOP_ATTR.c_str())));

    return new synthese::env::PhysicalStop (
	name, 
	rankInLogicalStop,
	environment.getLogicalStops ().get (logicalStopId));
}




XMLNode* 
PhysicalStopLS::Save (const synthese::env::PhysicalStop* physicalStop)
{
    // ...
}






}
}

