#include "ContinuousService.h"



namespace synthese
{
namespace env
{



ContinuousService::ContinuousService (const std::string& serviceNumber,
				      const Path* path,
				      Calendar* calendar,
				      const synthese::time::Schedule* departureSchedule,
				      int range,
				      int maxWaitingTime)
    : Service (serviceNumber, path, calendar, departureSchedule)
    , _range (range)
    , _maxWaitingTime (maxWaitingTime)
{

}
    


ContinuousService::~ContinuousService ()
{
}




    
int 
ContinuousService::getMaxWaitingTime () const
{
    return _maxWaitingTime;
}




int 
ContinuousService::getRange () const
{
    return _range;
}




bool 
ContinuousService::isContinuous () const
{
    return true;
}



}
}
