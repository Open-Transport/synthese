#include "ScheduledService.h"


namespace synthese
{
namespace env
{



ScheduledService::ScheduledService (const std::string& serviceNumber,
				    const Path* path,
				    Calendar* calendar,
				    const synthese::time::Schedule* departureSchedule)
    : Service (serviceNumber, path, calendar, departureSchedule)
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
