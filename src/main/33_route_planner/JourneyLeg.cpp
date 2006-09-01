#include "JourneyLeg.h"

#include "15_env/ContinuousService.h"
#include "15_env/Path.h"


using synthese::env::Axis;
using synthese::env::Path;
using synthese::env::ContinuousService;


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



const Axis* 
JourneyLeg::getAxis () const
{
    return _service->getPath ()->getAxis ();
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
    



int 
JourneyLeg::getContinuousServiceRange () const
{
    const ContinuousService* continuousService = dynamic_cast<const ContinuousService*> (_service);
    if (continuousService == 0) return 0;
    return continuousService->getRange ();
}




}
}
