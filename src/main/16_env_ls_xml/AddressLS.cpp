#include "AddressLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/Address.h"
#include "15_env/AddressablePlace.h"


using namespace synthese::util::XmlToolkit;

using synthese::env::Address;
using synthese::env::AddressablePlace;




namespace synthese
{
namespace envlsxml
{

const std::string AddressLS::ADDRESS_TAG ("address");
const std::string AddressLS::ADDRESS_ID_ATTR ("id");
const std::string AddressLS::ADDRESS_ROADID_ATTR ("roadId");
const std::string AddressLS::ADDRESS_METRICOFFSET_ATTR ("metricOffset");
const std::string AddressLS::ADDRESS_PLACEID_ATTR ("placeId");
const std::string AddressLS::ADDRESS_X_ATTR ("x");
const std::string AddressLS::ADDRESS_Y_ATTR ("y");
    


void 
AddressLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (ADDRESS_TAG == node.getName ());
    uid id (GetLongLongAttr (node, ADDRESS_ID_ATTR));

    if (environment.getAddresses ().contains (id)) return;

    uid roadId (GetLongLongAttr (node, ADDRESS_ROADID_ATTR));
    
    double metricOffset (GetDoubleAttr (node, ADDRESS_METRICOFFSET_ATTR));
    double x (GetDoubleAttr (node, ADDRESS_X_ATTR));
    double y (GetDoubleAttr (node, ADDRESS_Y_ATTR));

    const AddressablePlace* addressablePlace = 0;
    if (HasAttr (node, ADDRESS_PLACEID_ATTR))
    {
	uid placeId (GetLongLongAttr (node, ADDRESS_PLACEID_ATTR));
	addressablePlace = environment.fetchAddressablePlace (id);
    }

    Address* address = new synthese::env::Address (id, 
						   addressablePlace,
						   environment.getRoads ().get (roadId), 
						   metricOffset,
						   x,
						   y);

    environment.getAddresses ().add (address);

}




XMLNode* 
AddressLS::Save (const synthese::env::Address* address)
{
    // ...
    return 0;
}






}
}

