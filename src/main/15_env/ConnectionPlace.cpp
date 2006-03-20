#include "ConnectionPlace.h"

#include <limits>

namespace synthese
{
namespace env
{


const int ConnectionPlace::UNKNOWN_TRANSFER_DELAY = -1;
const int ConnectionPlace::FORBIDDEN_TRANSFER_DELAY = std::numeric_limits<int>::max ();




ConnectionPlace::ConnectionPlace (const std::string& name,
				  const City* city,
				  const ConnectionType& connectionType)
    : AddressablePlace (name, city)
    , _connectionType (connectionType)
{
}




ConnectionPlace::~ConnectionPlace ()
{
}



const ConnectionPlace::ConnectionType& 
ConnectionPlace::getConnectionType () const
{
    return _connectionType;
}



void 
ConnectionPlace::setConnectionType (const ConnectionType& connectionType)
{
    _connectionType = connectionType;
}





int 
ConnectionPlace::getTransferDelay (int departureRank, int arrivalRank) const
{
    if (_connectionType == CONNECTION_TYPE_FORBIDDEN) return FORBIDDEN_TRANSFER_DELAY;
    return _transferDelays.at (departureRank).at (arrivalRank);
}
 


void 
ConnectionPlace::setTransferDelay (int departureRank, int arrivalRank, int transferDelay)
{
    _transferDelays.at (departureRank).at (arrivalRank) = transferDelay;
}
    


}
}
