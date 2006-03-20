#include "Journey.h"
#include "JourneyLeg.h"

#include "04_time/DateTime.h"


namespace synthese
{
namespace routeplanner
{

Journey::Journey ()
{
}
    
Journey::~Journey ()
{
}



int 
Journey::getJourneyLegCount () const
{
    return _journeyLegs.size ();
}




const JourneyLeg* 
Journey::getJourneyLeg (int index) const
{
    return _journeyLegs.at (index);
}




const 
JourneyLeg* Journey::getFirstJourneyLeg () const
{
    return getJourneyLeg (0);
}



const JourneyLeg* 
Journey::getLastJourneyLeg () const
{
    return getJourneyLeg (getJourneyLegCount () - 1);
}



const synthese::env::Vertex* 
Journey::getOrigin() const
{
    return getFirstJourneyLeg ()->getOrigin ();
}



const synthese::env::Vertex* 
Journey::getDestination() const
{
    return getLastJourneyLeg ()->getDestination ();
}



const synthese::time::DateTime& 
Journey::getDepartureTime () const
{
    return getFirstJourneyLeg ()->getDepartureTime ();
}



const synthese::time::DateTime& 
Journey::getArrivalTime () const
{
    return getLastJourneyLeg ()->getArrivalTime ();
}




void 
Journey::clear ()
{
    _journeyLegs.clear ();
}



void 
Journey::prepend (const JourneyLeg* leg)
{
    _journeyLegs.push_front (leg);
}



void Journey::append (const JourneyLeg* leg)
{
    _journeyLegs.push_back (leg);
}




}
}
