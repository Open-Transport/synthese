#include "BestVertexReachesMap.h"

#include "JourneyLeg.h"


#include "15_env/ConnectionPlace.h"
#include "15_env/Vertex.h"

#include <assert.h>

using synthese::time::DateTime;
using synthese::env::Vertex;
using synthese::env::ConnectionPlace;



namespace synthese
{
namespace routeplanner
{


BestVertexReachesMap::BestVertexReachesMap (const synthese::env::AccessDirection& accessDirection)
    : _accessDirection (accessDirection)
{
    
}



BestVertexReachesMap::~BestVertexReachesMap ()
{
    
}



bool 
BestVertexReachesMap::contains (const synthese::env::Vertex* vertex) const
{
    return (_vertexMap.find (vertex) != _vertexMap.end ());
}






void 
BestVertexReachesMap::insert (const synthese::env::Vertex* vertex, JourneyLeg* journeyLeg)
{
    // Update vertex map
    // Implementation note : journey legs are reused and never re-allocated.
    // Thus the insertion happens only once
    assert (contains (vertex) == false);
    _vertexMap.insert (std::make_pair (vertex, journeyLeg));

    // Update connection place map (replacement)
    const ConnectionPlace* connectionPlace = vertex->getConnectionPlace ();

    if (connectionPlace == 0) return;

    ConnectionPlaceMap::iterator itc = _connectionPlaceMap.find (connectionPlace);

    DateTime bestTime;
    if (_accessDirection == synthese::env::TO_DESTINATION)
    {
	bestTime = journeyLeg->getArrivalTime ();
	bestTime += connectionPlace->getMaxTransferDelay ();
    }
    else
    {
	bestTime = journeyLeg->getDepartureTime ();
	bestTime -= connectionPlace->getMaxTransferDelay ();
    }
    
    if (itc == _connectionPlaceMap.end ()) 
    {
	_connectionPlaceMap.insert (std::make_pair (connectionPlace, bestTime));
    }
    else
    {
	if (bestTime < itc->second)
	{
	    itc->second = bestTime;
	}
    }
}    
    






const DateTime& 
BestVertexReachesMap::getBestTime (const Vertex* vertex, 
				   const DateTime& defaultValue) const
{
    if (contains (vertex)) 
    {
	return (_accessDirection == synthese::env::TO_DESTINATION) ? 
	    _vertexMap.find (vertex)->second->getArrivalTime () : 
	    _vertexMap.find (vertex)->second->getDepartureTime ();
    }
    else if (vertex->getConnectionPlace ())
    {
	ConnectionPlaceMap::const_iterator itc = 
	    _connectionPlaceMap.find (vertex->getConnectionPlace ());

	if (itc != _connectionPlaceMap.end ())
	{
	    return itc->second;
	}
    }

    return defaultValue;

}



JourneyLeg*
BestVertexReachesMap::getBestJourneyLeg (const Vertex* vertex)
{
    if (contains (vertex) == false) return 0;
    return _vertexMap.find (vertex)->second;
}




}

}

