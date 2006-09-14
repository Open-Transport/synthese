#include "PedestrianComplianceLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/HandicappedCompliance.h"


using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{


const std::string PedestrianComplianceLS::PEDESTRIANCOMPLIANCE_TAG ("pedestrianCompliance");
const std::string PedestrianComplianceLS::PEDESTRIANCOMPLIANCE_ID_ATTR ("id");
const std::string PedestrianComplianceLS::PEDESTRIANCOMPLIANCE_STATUS_ATTR ("status");
const std::string PedestrianComplianceLS::PEDESTRIANCOMPLIANCE_CAPACITY_ATTR ("capacity");




void 
PedestrianComplianceLS::Load (XMLNode& node,
			synthese::env::Environment& environment)
{
    // assert (PEDESTRIANCOMPLIANCE_TAG == node.getName ());
    uid id (GetLongLongAttr (node, PEDESTRIANCOMPLIANCE_ID_ATTR));

    if (environment.getPedestrianCompliances ().contains (id)) return;

    boost::logic::tribool status (GetTriboolAttr (node, PEDESTRIANCOMPLIANCE_STATUS_ATTR));
    int capacity (GetIntAttr (node, PEDESTRIANCOMPLIANCE_CAPACITY_ATTR));
    
    environment.getPedestrianCompliances ().add (
	new synthese::env::PedestrianCompliance (id, status, capacity));
}




XMLNode* 
PedestrianComplianceLS::Save (const synthese::env::PedestrianCompliance* axis)
{
    // ...
    return 0;
}






}
}

