#include "BestVertexReachesMap.h"

#include "JourneyLeg.h"


#include "15_env/ConnectionPlace.h"
#include "15_env/Address.h"
#include "15_env/PhysicalStop.h"
#include "15_env/Vertex.h"

#include <assert.h>

using synthese::time::DateTime;
using synthese::env::Vertex;
using synthese::env::Address;
using synthese::env::PhysicalStop;
using synthese::env::ConnectionPlace;

using synthese::env::TO_DESTINATION;
using synthese::env::FROM_ORIGIN;


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
    return (_bestJourneyLegMap.find (vertex) != _bestJourneyLegMap.end ());
}






void 
BestVertexReachesMap::insert (const synthese::env::Vertex* vertex, JourneyLeg* journeyLeg)
{
    // Update journey leg map
    // Implementation note : journey legs are reused and never re-allocated.
    // Thus the insertion happens only once
    assert (contains (vertex) == false);
    _bestJourneyLegMap.insert (std::make_pair (vertex, journeyLeg));

    // Update time map (replacement)
    if (_accessDirection == synthese::env::TO_DESTINATION)
    {
	insert (vertex, journeyLeg->getArrivalTime ());
    }
    else
    {
	insert (vertex, journeyLeg->getDepartureTime ());
    }
    
}    
    


void 
BestVertexReachesMap::insert (const synthese::env::Vertex* vertex, 
			      const synthese::time::DateTime& dateTime,
			      bool propagateInConnectionPlace)
{
    TimeMap::iterator itc = _bestTimeMap.find (vertex);
    DateTime bestTime = dateTime;

    if (itc == _bestTimeMap.end ()) 
    {
	_bestTimeMap.insert (std::make_pair (vertex, bestTime));
    }
    else
    {
	// TODO : rename FROM_ORIGIN into TOWARD_ORIGIN
	// TODO : rename TO_DESTINATION into TOWARD_DESTINATION
	if ( (_accessDirection == TO_DESTINATION) && (bestTime < itc->second) ||
	     (_accessDirection == FROM_ORIGIN) && (bestTime > itc->second) )
	{
	    itc->second = bestTime;
	}
	else
	{
	    bestTime = itc->second;
	}
    }

    if (propagateInConnectionPlace)
    {
	const ConnectionPlace* cp = vertex->getConnectionPlace ();
	assert (cp != 0);

	DateTime bestTimeUpperBound (bestTime);

	// TODO : Could be more accurate (with a per vertex max transfer delay)
	// TODO : Also check special forbidden transfer delays
	if (_accessDirection == TO_DESTINATION)
	{
	    bestTimeUpperBound += cp->getMaxTransferDelay ();
	}
	else
	{
	    bestTimeUpperBound -= cp->getMaxTransferDelay ();
	}

	
	for (std::vector<const Address*>::const_iterator ita = cp->getAddresses ().begin ();
	     ita != cp->getAddresses ().end (); ++ita)
	{
	    insert (*ita, bestTimeUpperBound, false);
	}
	for (std::vector<const PhysicalStop*>::const_iterator itp = cp->getPhysicalStops ().begin ();
	     itp != cp->getPhysicalStops ().end (); ++itp)
	{
	    insert (*itp, bestTimeUpperBound, false);
	}

    }

}





const DateTime& 
BestVertexReachesMap::getBestTime (const Vertex* vertex, 
				   const DateTime& defaultValue) const
{
    TimeMap::const_iterator itc = 
	_bestTimeMap.find (vertex);
    
    if (itc != _bestTimeMap.end ())
    {
	return itc->second;
    }

    
    return defaultValue;

}



JourneyLeg*
BestVertexReachesMap::getBestJourneyLeg (const Vertex* vertex)
{
    if (contains (vertex) == false) return 0;
    return _bestJourneyLegMap.find (vertex)->second;
}



void 
BestVertexReachesMap::clear ()
{
    _bestJourneyLegMap.clear ();
    _bestTimeMap.clear ();
}




}

}

