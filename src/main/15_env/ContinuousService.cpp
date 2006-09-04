#include "ContinuousService.h"



namespace synthese
{
namespace env
{



ContinuousService::ContinuousService (const uid& id,
				      int serviceNumber,
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




void 
ContinuousService::setMaxWaitingTime (int maxWaitingTime)
{
    _maxWaitingTime = maxWaitingTime;
}




int 
ContinuousService::getRange () const
{
    return _range;
}

void 
ContinuousService::setRange (int range)
{
    _range = range;
}



bool 
ContinuousService::isContinuous () const
{
    return true;
}



}
}
