#include "AddressablePlace.h"

#include <assert.h>

namespace synthese
{
namespace env
{

AddressablePlace::AddressablePlace (const std::string& name,
				    const City* city)
    : Place (name, city)
{
}



AddressablePlace::~AddressablePlace ()
{

}



const std::vector<const Address*>& 
AddressablePlace::getAddresses () const
{
    return _addresses;
}
    


void 
AddressablePlace::addAddress (const Address* address)
{
    _addresses.push_back (address);
}



void 
AddressablePlace::reachPhysicalStopAccesses (const AccessDirection& accessDirection,
					     const AccessParameters& accessParameters,
					     PhysicalStopAccessMap& result) const
{
    assert (false); // TODO
}





}
}
