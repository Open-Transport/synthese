#include "Alarm.h"


namespace synthese
{
namespace env
{


Alarm::Alarm (const uid& id,
	      const std::string& message, 
	      const synthese::time::DateTime& periodStart,
	      const synthese::time::DateTime& periodEnd,
	      const AlarmLevel& level)
    : synthese::util::Registrable<uid,Alarm> (id)
    , _message (message)
    , _periodStart (periodStart)
    , _periodEnd (periodEnd)
    , _level (level)
{
    _periodStart.updateDateTime( synthese::time::TIME_MIN );
    _periodEnd.updateDateTime( synthese::time::TIME_MAX );
}
    



const std::string& 
Alarm::getMessage () const
{
    return _message;
}


void 
Alarm::setMessage( const std::string& message)
{
    _message = message;
}



const Alarm::AlarmLevel& 
Alarm::getLevel () const
{
    return _level;
}



void 
Alarm::setLevel (const AlarmLevel& level)
{
    _level = level;
}


void 
Alarm::setPeriodStart ( const synthese::time::DateTime& periodStart)
{
    _periodStart = periodStart;
}


void 
Alarm::setPeriodEnd ( const synthese::time::DateTime& periodEnd)
{
    _periodEnd = periodEnd;
}




bool 
Alarm::isMessageShowable ( const synthese::time::DateTime& start, 
			   const synthese::time::DateTime& end ) const
{
    if ( _message.empty () )
        return false;

    if ( ( start < _periodStart ) || 
	 ( end > _periodEnd ) )
        return false;
    
    return true;
}











}
}
