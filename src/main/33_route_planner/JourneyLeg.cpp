#include "JourneyLeg.h"

#include "15_env/ContinuousService.h"
#include "15_env/Edge.h"
#include "15_env/Path.h"
#include "15_env/SquareDistance.h"


using synthese::env::Axis;
using synthese::env::Edge;
using synthese::env::Path;
using synthese::env::Service;
using synthese::env::SquareDistance;
using synthese::env::ContinuousService;


namespace synthese
{
namespace routeplanner
{


JourneyLeg::JourneyLeg ()
    : _origin (0)
    , _destination (0)
    , _departureTime ()
    , _arrivalTime ()
    , _service (0)
    , _continuousServiceRange (0)
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



void 
JourneyLeg::setService (const Service* service)
{
    _service = service;
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


void 
JourneyLeg::setDepartureTime (const synthese::time::DateTime& departureTime)
{
    _departureTime = departureTime;
}



const synthese::time::DateTime& 
JourneyLeg::getArrivalTime () const
{
    return _arrivalTime;
}



void 
JourneyLeg::setArrivalTime (const synthese::time::DateTime& arrivalTime)
{
    _arrivalTime = arrivalTime;
}






const synthese::env::Edge* 
JourneyLeg::getOrigin () const
{
    return _origin;
}




void 
JourneyLeg::setOrigin (const synthese::env::Edge* origin)
{
    _origin = origin;
}





const synthese::env::Edge* 
JourneyLeg::getDestination () const
{
    return _destination;
}
    



void 
JourneyLeg::setDestination (const synthese::env::Edge* destination)
{
    _destination = destination;
}





int 
JourneyLeg::getContinuousServiceRange () const
{
    return _continuousServiceRange;
}




void 
JourneyLeg::setContinuousServiceRange (int continuousServiceRange)
{
    _continuousServiceRange = continuousServiceRange;
}





const SquareDistance& 
JourneyLeg::getSquareDistance () const
{
    return _squareDistance;
}




synthese::env::SquareDistance& 
JourneyLeg::getSquareDistance ()
{
    return _squareDistance;
}




void 
JourneyLeg::setSquareDistance (const SquareDistance& squareDistance)
{
    _squareDistance.setSquareDistance (squareDistance.getSquareDistance ());
}




int 
JourneyLeg::getDuration () const
{
    return _arrivalTime - _departureTime;
}




int 
JourneyLeg::getDistance () const
{
    return _destination->getMetricOffset () -
	_origin->getMetricOffset ();
}



const Path* 
JourneyLeg::getPath () const
{
    return _origin->getParentPath ();
}



}
}

