#include "ContinuousService.h"



namespace synthese
{
namespace env
{



ContinuousService::ContinuousService (const uid& id,
				      const std::string& serviceNumber,
				      const Path* path,
				      const synthese::time::Schedule& departureSchedule,
				      int range,
				      int maxWaitingTime)
    : synthese::util::Registrable<uid,ContinuousService> (id)
    , Service (serviceNumber, path, departureSchedule)
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
