#include "Service.h"

#include "Path.h"



namespace synthese
{
namespace env
{


    
Service::Service (const std::string& serviceNumber,
		  const Path* path,
		  const synthese::time::Schedule& departureSchedule)
    : BikeComplyer (path) 
    , HandicappedComplyer (path) 
    , PedestrianComplyer (path) 
    , _serviceNumber (serviceNumber)
    , _path (path)
    , _departureSchedule (departureSchedule)
{
}



Service::~Service ()
{
}



const std::string& 
Service::getServiceNumber () const
{
    return _serviceNumber;
}



const Path* 
Service::getPath () const
{
    return _path;
}



Calendar&
Service::getCalendar ()
{
    return _calendar;
}



const synthese::time::Schedule&
Service::getDepartureSchedule () const
{
    return _departureSchedule;
}



void 
Service::setDepartureSchedule (const synthese::time::Schedule& departureSchedule)
{
    _departureSchedule = departureSchedule;
}



bool 
Service::isReservationPossible ( const synthese::time::DateTime& departureMoment, 
				 const synthese::time::DateTime& calculationMoment ) const
{
    return false;
}





bool 
Service::isProvided ( const synthese::time::Date& departureDate,
		      int jplus ) const
{
    if ( jplus > 0 )
    {
        synthese::time::Date _originDepartureDate;
        _originDepartureDate = departureDate;
        _originDepartureDate -= jplus;
        return _calendar.isMarked( _originDepartureDate );
    }
    else
        return _calendar.isMarked ( departureDate );
}






}
}
