#include "HandicappedComplianceLS.h"

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


const std::string HandicappedComplianceLS::HANDICAPPEDCOMPLIANCE_TAG ("handicappedCompliance");
const std::string HandicappedComplianceLS::HANDICAPPEDCOMPLIANCE_ID_ATTR ("id");
const std::string HandicappedComplianceLS::HANDICAPPEDCOMPLIANCE_STATUS_ATTR ("status");
const std::string HandicappedComplianceLS::HANDICAPPEDCOMPLIANCE_CAPACITY_ATTR ("capacity");




void 
HandicappedComplianceLS::Load (XMLNode& node,
			synthese::env::Environment& environment)
{
    // assert (HANDICAPPEDCOMPLIANCE_TAG == node.getName ());
    uid id (GetLongLongAttr (node, HANDICAPPEDCOMPLIANCE_ID_ATTR));

    if (environment.getHandicappedCompliances ().contains (id)) return;

    boost::logic::tribool status (GetTriboolAttr (node, HANDICAPPEDCOMPLIANCE_STATUS_ATTR));
    int capacity (GetIntAttr (node, HANDICAPPEDCOMPLIANCE_CAPACITY_ATTR));
    
    environment.getHandicappedCompliances ().add (
	new synthese::env::HandicappedCompliance (id, status, capacity));
}




XMLNode* 
HandicappedComplianceLS::Save (const synthese::env::HandicappedCompliance* axis)
{
    // ...
    return 0;
}






}
}

