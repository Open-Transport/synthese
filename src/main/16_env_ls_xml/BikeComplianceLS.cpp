#include "BikeComplianceLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/BikeCompliance.h"


using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{


const std::string BikeComplianceLS::BIKECOMPLIANCE_TAG ("bikeCompliance");
const std::string BikeComplianceLS::BIKECOMPLIANCE_ID_ATTR ("id");
const std::string BikeComplianceLS::BIKECOMPLIANCE_STATUS_ATTR ("status");
const std::string BikeComplianceLS::BIKECOMPLIANCE_CAPACITY_ATTR ("capacity");




void 
BikeComplianceLS::Load (XMLNode& node,
			synthese::env::Environment& environment)
{
    // assert (BIKECOMPLIANCE_TAG == node.getName ());
    uid id (GetLongLongAttr (node, BIKECOMPLIANCE_ID_ATTR));

    if (environment.getBikeCompliances ().contains (id)) return;

    boost::logic::tribool status (GetTriboolAttr (node, BIKECOMPLIANCE_STATUS_ATTR));
    int capacity (GetIntAttr (node, BIKECOMPLIANCE_CAPACITY_ATTR));
    
    environment.getBikeCompliances ().add (
	new synthese::env::BikeCompliance (id, status, capacity));
}




XMLNode* 
BikeComplianceLS::Save (const synthese::env::BikeCompliance* axis)
{
    // ...
    return 0;
}






}
}


