#include "JourneyLegComparator.h"


#include "JourneyLeg.h"
#include "15_env/Edge.h"
#include "15_env/Vertex.h"
#include "15_env/ConnectionPlace.h"

#include <assert.h>


using synthese::env::ConnectionPlace;




namespace synthese
{
namespace routeplanner
{



JourneyLegComparator::JourneyLegComparator (const synthese::env::AccessDirection& accessDirection)
    : _edgeAccessor ( (accessDirection == synthese::env::TO_DESTINATION)
		      ? (&JourneyLeg::getDestination)
		      : (&JourneyLeg::getOrigin) ) 
{

}
 

JourneyLegComparator::~JourneyLegComparator ()
{

}




int 
JourneyLegComparator::operator () (const JourneyLeg* jl1, const JourneyLeg* jl2) const
{

    if (jl1->getSquareDistance () == 0) return true;
    if (jl2->getSquareDistance () == 0) return false;
    
    if (jl1->getSquareDistance () == jl2->getSquareDistance ()) return false;

    assert ((jl1->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace () != 0);
    assert ((jl2->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace () != 0);

    ConnectionPlace::ConnectionType type1 = 
	(jl1->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace ()
	->getRecommendedConnectionType (jl1->getSquareDistance ());

    ConnectionPlace::ConnectionType type2 = 
	(jl2->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace ()
	->getRecommendedConnectionType (jl2->getSquareDistance ());
    
    if (type1 != type2)	return type2 - type1;

    return (jl2->getSquareDistance () <= jl1->getSquareDistance ());
}








}

}
