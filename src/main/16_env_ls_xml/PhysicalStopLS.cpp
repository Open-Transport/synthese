#include "PhysicalStopLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/PhysicalStop.h"

#include "PointLS.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string PhysicalStopLS::PHYSICALSTOP_TAG ("physicalStop");
const std::string PhysicalStopLS::PHYSICALSTOP_ID_ATTR ("id");
const std::string PhysicalStopLS::PHYSICALSTOP_NAME_ATTR ("name");
const std::string PhysicalStopLS::PHYSICALSTOP_CONNECTIONPLACEID_ATTR ("connectionPlaceId");


synthese::env::PhysicalStop* 
PhysicalStopLS::Load (XMLNode& node,
		      const synthese::env::Environment& environment)
{
    // assert (PHYSICALSTOP_TAG == node.getName ());

    uid id (su::Conversion::ToLongLong (
		node.getAttribute (PHYSICALSTOP_ID_ATTR.c_str())));
    std::string name (node.getAttribute (PHYSICALSTOP_NAME_ATTR.c_str()));
    int connectionPlaceId (su::Conversion::ToInt (
	      node.getAttribute (PHYSICALSTOP_CONNECTIONPLACEID_ATTR.c_str())));

    double x (su::Conversion::ToDouble (
		node.getAttribute (PointLS::POINT_X_ATTR.c_str())));
    double y (su::Conversion::ToDouble (
		node.getAttribute (PointLS::POINT_Y_ATTR.c_str())));

    return new synthese::env::PhysicalStop (
	id,
	name, 
	environment.getConnectionPlaces ().get (connectionPlaceId), 
	x, y);
}




XMLNode* 
PhysicalStopLS::Save (const synthese::env::PhysicalStop* physicalStop)
{
    // ...
    return 0;

}






}
}

