#include "PhysicalStopLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/PhysicalStop.h"

using namespace synthese::util::XmlToolkit;





namespace synthese
{
namespace envlsxml
{

const std::string PhysicalStopLS::PHYSICALSTOP_TAG ("physicalStop");
const std::string PhysicalStopLS::PHYSICALSTOP_ID_ATTR ("id");
const std::string PhysicalStopLS::PHYSICALSTOP_NAME_ATTR ("name");
const std::string PhysicalStopLS::PHYSICALSTOP_PLACEID_ATTR ("placeId");
const std::string PhysicalStopLS::PHYSICALSTOP_X_ATTR ("x");
const std::string PhysicalStopLS::PHYSICALSTOP_Y_ATTR ("y");



void
PhysicalStopLS::Load (XMLNode& node,
		      synthese::env::Environment& environment)
{
    // assert (PHYSICALSTOP_TAG == node.getName ());

    uid id (GetLongLongAttr (node, PHYSICALSTOP_ID_ATTR));

    if (environment.getPhysicalStops ().contains (id)) return;

    std::string name (GetStringAttr (node, PHYSICALSTOP_NAME_ATTR));
    uid placeId (GetLongLongAttr (node, PHYSICALSTOP_PLACEID_ATTR));

    double x (GetDoubleAttr (node, PHYSICALSTOP_X_ATTR));
    double y (GetDoubleAttr (node, PHYSICALSTOP_Y_ATTR));

    environment.getPhysicalStops ().add (new synthese::env::PhysicalStop (
					     id,
					     name, 
					     environment.getConnectionPlaces ().get (placeId), 
					     x, y));

}




XMLNode* 
PhysicalStopLS::Save (const synthese::env::PhysicalStop* physicalStop)
{
    // ...
    return 0;

}






}
}


