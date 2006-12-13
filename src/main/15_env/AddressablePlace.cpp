#include "AddressablePlace.h"

#include "Address.h"

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






const AddressablePlace::ConnectionType 
AddressablePlace::getConnectionType () const
{
    return CONNECTION_TYPE_FORBIDDEN;
}





void
AddressablePlace::getImmediateVertices (VertexAccessMap& result, 
					const AccessDirection& accessDirection,
					const AccessParameters& accessParameters,
					const Vertex* origin,
					bool returnAddresses,
					bool returnPhysicalStops) const
{
    if (returnAddresses)
    {
	for (std::vector<const Address*>::const_iterator it = _addresses.begin ();
	     it != _addresses.end (); ++it)
	{
	    if (origin == (*it)) continue;
	    result.insert ((*it), getVertexAccess (accessDirection,
						   accessParameters,
						   (*it), origin));
	}
    }

}







}
}

