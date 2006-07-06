#include "ScheduledService.h"


namespace synthese
{
namespace env
{



ScheduledService::ScheduledService (const uid& id,
				    const std::string& serviceNumber,
				    const Path* path,
				    Calendar* calendar,
				    const synthese::time::Schedule& departureSchedule)
    : synthese::util::Registrable<uid,ScheduledService> (id)
    , Service (serviceNumber, path, calendar, departureSchedule)
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









}
}
