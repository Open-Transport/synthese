#include "JourneyLeg.h"



namespace synthese
{
namespace routeplanner
{


JourneyLeg::JourneyLeg (const synthese::env::Vertex* origin,
			const synthese::env::Vertex* destination,
			const synthese::time::DateTime& departureTime,
			const synthese::time::DateTime& arrivalTime,
			const synthese::env::Service* service)
    : _origin (origin)
    , _destination (destination)
    , _departureTime (departureTime)
    , _arrivalTime (arrivalTime)
    , _service (service)
{
}



JourneyLeg::~JourneyLeg ()
{
}





const synthese::env::Service* 
JourneyLeg::getService () const
{
    return _service;
}






const synthese::time::DateTime& 
JourneyLeg::getDepartureTime () const
{
    return _departureTime;
}




const synthese::time::DateTime& 
JourneyLeg::getArrivalTime () const
{
    return _arrivalTime;
}




const synthese::env::Vertex* 
JourneyLeg::getOrigin () const
{
    return _origin;
}



const synthese::env::Vertex* 
JourneyLeg::getDestination () const
{
    return _destination;
}
    


}
}
