#include "ScheduledService.h"

#include "Path.h"
#include "ReservationRule.h"



namespace synthese
{
namespace env
{



ScheduledService::ScheduledService (const uid& id,
				    int serviceNumber,
				    const Path* path,
				    const synthese::time::Schedule& departureSchedule)
    : synthese::util::Registrable<uid,ScheduledService> (id)
    , ReservationRuleComplyer (path) 
    , Service (serviceNumber, path, departureSchedule)
{

}



ScheduledService::~ScheduledService ()
{
}



    
bool 
ScheduledService::isContinuous () const
{
    return false;
}



bool 
ScheduledService::isReservationPossible ( const synthese::time::DateTime& departureMoment, 
					  const synthese::time::DateTime& calculationMoment ) const
{
    if (getReservationRule () == 0) return true;

    return getReservationRule ()->isRunPossible 
	(this, calculationMoment, departureMoment );
    
}







}
}
