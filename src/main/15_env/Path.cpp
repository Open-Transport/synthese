#include "Path.h"

#include "Service.h"


#include <assert.h>


namespace synthese
{
namespace env
{



Path::Path ()
    : BikeComplyer (0) // No parent complyer right now
    , HandicappedComplyer (0) // No parent complyer right now
    , PedestrianComplyer (0) // No parent complyer right now
    , ReservationRuleComplyer (0) // No parent complyer right now
      , _calendar (2006, 2006, 0, "") // MJ : todo review 
{
    
}
    


Path::~Path ()
{
}





const Alarm* 
Path::getAlarm() const
{
    return _alarm;
}



const Fare* 
Path::getFare () const
{
    return _fare;
}



void 
Path::setFare (Fare* fare)
{
    _fare = fare;
}





const std::vector<Service*>& 
Path::getServices () const
{
    return _services;
}




const Service* 
Path::getService (int serviceNumber) const
{
    return _services.at (serviceNumber);
}




void 
Path::addService (Service* service)
{
    _services.push_back (service);
}




bool 
Path::isInService (const synthese::time::Date& date) const
{
    return _calendar.isMarked ( date );

}




void 
Path::updateCalendar ()
{
    /* MJ TODO REVIEW this

    _calendar.reset ();
    LineStop* lastLineStop = _lineStops.back();
    for (int s=0; s<_services.size (); ++s)
    {
        if ( lastLineStop->getLastArrivalSchedule (s).getDaysSinceDeparture() != 
	     _lineStops.front()->getFirstDepartureSchedule(s).getDaysSinceDeparture() )
        {
            _calendar.reset (true);
            break;
        }
	// MJ constness problem !
        ((Service*) getService (s))->getCalendar ()->setInclusionToMask (_calendar);
    }
    */

}








}
}
