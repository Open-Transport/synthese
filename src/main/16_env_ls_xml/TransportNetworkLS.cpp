#include "TransportNetworkLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/TransportNetwork.h"


using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{

const std::string TransportNetworkLS::TRANSPORTNETWORK_TAG ("transportNetwork");
const std::string TransportNetworkLS::TRANSPORTNETWORK_ID_ATTR ("id");
const std::string TransportNetworkLS::TRANSPORTNETWORK_NAME_ATTR ("name");


void
TransportNetworkLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    uid id (GetLongLongAttr (node, TRANSPORTNETWORK_ID_ATTR));

    if (environment.getTransportNetworks ().contains (id)) return;

    std::string name (GetStringAttr (node, TRANSPORTNETWORK_NAME_ATTR));

    environment.getTransportNetworks ().add (new synthese::env::TransportNetwork (id, name));
}




XMLNode* 
TransportNetworkLS::Save (const synthese::env::TransportNetwork* city)
{
    // ...
    return 0;
}






}
}


