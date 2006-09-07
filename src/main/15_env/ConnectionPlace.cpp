#include "ConnectionPlace.h"

#include "04_time/DateTime.h"

#include "15_env/Alarm.h"
#include "15_env/PhysicalStop.h"
#include "15_env/SquareDistance.h"
#include "15_env/Vertex.h"
#include "15_env/Line.h"
#include "15_env/Vertex.h"


#include <limits>
#include <assert.h>

namespace synthese
{
namespace env
{


const int ConnectionPlace::UNKNOWN_TRANSFER_DELAY = -1;
const int ConnectionPlace::FORBIDDEN_TRANSFER_DELAY = std::numeric_limits<int>::max ();
const int ConnectionPlace::SQUAREDISTANCE_SHORT_LONG = 625;



ConnectionPlace::ConnectionPlace (const uid& id,
				  const std::string& name,
				  const City* city,
				  const ConnectionType& connectionType,
				  int defaultTransferDelay)
    : synthese::util::Registrable<uid,ConnectionPlace> (id)
    , AddressablePlace (name, city)
    , _connectionType (connectionType)
    , _defaultTransferDelay (defaultTransferDelay)
    , _minTransferDelay (std::numeric_limits<int>::max ())
    , _maxTransferDelay (0)
    , _alarm (0)
{
}




ConnectionPlace::~ConnectionPlace ()
{
}




const AddressablePlace::ConnectionType
ConnectionPlace::getConnectionType () const
{
    return _connectionType;
}







int 
ConnectionPlace::getDefaultTransferDelay () const
{
    return _defaultTransferDelay;
}





int 
ConnectionPlace::getMinTransferDelay () const
{
    return _minTransferDelay;
}


int 
ConnectionPlace::getMaxTransferDelay () const
{
    return _maxTransferDelay;
}






void 
ConnectionPlace::setDefaultTransferDelay (int defaultTransferDelay)
{
    _defaultTransferDelay = defaultTransferDelay;
}






void 
ConnectionPlace::setConnectionType (const ConnectionType& connectionType)
{
    _connectionType = connectionType;
}




int 
ConnectionPlace::getTransferDelay (const Vertex* fromVertex, 
				   const Vertex* toVertex) const
{
    std::map< std::pair<uid, uid>, int >::const_iterator iter = 
	_transferDelays.find (std::make_pair (fromVertex->getId (), 
					      toVertex->getId ()));
    
    // If not defined in map, return default transfer delay
    if (iter == _transferDelays.end ()) return _defaultTransferDelay;
    return iter->second;
}
 


void 
ConnectionPlace::addTransferDelay (uid departureId, uid arrivalId, int transferDelay)
{
    _transferDelays[std::make_pair (departureId, arrivalId)] = transferDelay;
    if (transferDelay < _minTransferDelay)
    {
	_minTransferDelay = transferDelay;
    }
    if (transferDelay < _maxTransferDelay)
    {
	_maxTransferDelay = transferDelay;
    }
}

    

void 
ConnectionPlace::clearTransferDelays ()
{
    _transferDelays.clear ();
}




void 
ConnectionPlace::addPhysicalStop (const PhysicalStop* physicalStop)
{
    _physicalStops.push_back (physicalStop);
}



bool 
ConnectionPlace::hasApplicableAlarm (const synthese::time::DateTime& start, 
				     const synthese::time::DateTime& end) const
{
    if (_alarm == 0) return false;
    return _alarm->isApplicable (start, end);
}



const Alarm* 
ConnectionPlace::getAlarm () const
{
    return _alarm;
}




void 
ConnectionPlace::setAlarm (const Alarm* alarm)
{
    _alarm = alarm;
}






VertexAccess 
ConnectionPlace::getVertexAccess (const AccessDirection& accessDirection,
				  const AccessParameters& accessParameters,
				  const Vertex* destination,
				  const Vertex* origin) const
{
    VertexAccess access;

    if (origin != 0)
    {
	access.approachDistance = 0;
	if (accessDirection == FROM_ORIGIN)
	{
	    access.approachTime = getTransferDelay (origin, destination);
	} 
	else
	{
	    access.approachTime = getTransferDelay (destination, origin);
	}
    }
    else
    {
	access.approachDistance = 0;
	access.approachTime = 0;
    }

    return access;
}
    






void
ConnectionPlace::getImmediateVertices (VertexAccessMap& result, 
				       const AccessDirection& accessDirection,
				       const AccessParameters& accessParameters,
				       const Vertex* origin,
				       bool returnAddresses,
				       bool returnPhysicalStops) const
{
    AddressablePlace::getImmediateVertices (result, accessDirection, accessParameters,
					    origin, returnAddresses, returnPhysicalStops);
    
    if (returnPhysicalStops)
    {
	for (std::vector<const PhysicalStop*>::const_iterator it = _physicalStops.begin ();
	     it != _physicalStops.end (); ++it)
	{
	    if (origin == (*it)) continue;
	    result.insert ((*it), getVertexAccess (accessDirection,
						   accessParameters,
						   (*it), origin));
	}
    }
}




    
ConnectionPlace::ConnectionType 
ConnectionPlace::getRecommendedConnectionType (const SquareDistance& squareDistance) const
{
    if (_connectionType == CONNECTION_TYPE_RECOMMENDED_SHORT)
    {
	return (squareDistance.getSquareDistance () > SQUAREDISTANCE_SHORT_LONG) 
	    ? CONNECTION_TYPE_LINELINE
	    : CONNECTION_TYPE_RECOMMENDED ;
	
    }
    return _connectionType;

}




bool 
ConnectionPlace::isConnectionAllowed (const Vertex* fromVertex, 
				      const Vertex* toVertex) const
{
    if (_connectionType == CONNECTION_TYPE_FORBIDDEN) return false;
    
    bool fromVertexOnLine (dynamic_cast<const PhysicalStop*> (fromVertex));
    bool toVertexOnLine (dynamic_cast<const PhysicalStop*> (toVertex));

    if ( (_connectionType == CONNECTION_TYPE_ROADROAD) &&
	 (fromVertexOnLine == false) &&
	 (toVertexOnLine == false) ) return true;

    if ( (_connectionType == CONNECTION_TYPE_ROADLINE) &&
	 ((fromVertexOnLine == false) || (toVertexOnLine == false)) ) return true;
    
    if (_connectionType >= CONNECTION_TYPE_LINELINE) 
    {
	return getTransferDelay (fromVertex,
				 toVertex) != FORBIDDEN_TRANSFER_DELAY;
    }
    
    return false;
}







}
}
