
#include "Schedule.h"
#include "assert.h"

#include "01_util/Conversion.h"


using synthese::util::Conversion;


namespace synthese
{
namespace time
{


  Schedule::Schedule () 
  {
  }


Schedule::Schedule ( const Hour& hour, int daysSinceDeparture )
        : _hour ( hour )
        , _daysSinceDeparture ( daysSinceDeparture )
{
    assert ( ( _daysSinceDeparture >= 0 ) &&
             ( _daysSinceDeparture <= 28 ) );
}



Schedule::Schedule ( const Schedule& ref )
        : _hour ( ref._hour )
        , _daysSinceDeparture ( ref._daysSinceDeparture )
{
    assert ( ( _daysSinceDeparture >= 0 ) &&
             ( _daysSinceDeparture <= 28 ) );

}


Schedule::~Schedule ()
{}


const Hour&
Schedule::getHour() const
{
    return _hour;
}



int
Schedule::getHours() const
{
    return _hour.getHours ();
}



int
Schedule::getMinutes() const
{
    return _hour.getMinutes ();
}



int
Schedule::getDaysSinceDeparture () const
{
    return _daysSinceDeparture;
}


void
Schedule::setDaysSinceDeparture ( int daysSinceDeparture )
{
    _daysSinceDeparture = daysSinceDeparture;
}


bool 
Schedule::isValid () const
{
  return(_daysSinceDeparture >= 0 && 
	 _daysSinceDeparture <=28 && 
	 _hour.isValid ());
}



void
Schedule::setMinimum()
{
    _daysSinceDeparture = 0;
    _hour.updateHour ( TIME_MIN, TIME_MIN );
}


void
Schedule::setMaximum()
{
    _daysSinceDeparture = 255;
    _hour.updateHour( TIME_MAX, TIME_MAX );
}


Schedule&
Schedule::operator += ( int op )
{
    _daysSinceDeparture = ( _daysSinceDeparture + ( _hour += op ) );
    return ( *this );
}




Schedule
operator + ( const Schedule& op1, int op2 )
{
    Schedule result = op1;
    result += op2;
    return result;
}






bool
operator < ( const Schedule& op1, const Schedule& op2 )
{
    return ( op1.getDaysSinceDeparture () < op2.getDaysSinceDeparture () ||
             op1.getDaysSinceDeparture () == op2.getDaysSinceDeparture () &&
             op1.getHour() < op2.getHour () );
}



bool
operator < ( const Schedule& op1, const Hour& op2 )
{
    return ( op1.getHour () < op2 );
}


bool operator <= ( const Schedule& op1, const Schedule& op2 )
{
    return ( op1.getDaysSinceDeparture () < op2.getDaysSinceDeparture () ||
             op1.getDaysSinceDeparture () == op2.getDaysSinceDeparture () &&
             op1.getHour () <= op2.getHour () );

}



bool
operator <= ( const Schedule& op1, const Hour& op2 )
{
    return ( op1.getHour () <= op2 );

}



bool
operator >= ( const Schedule& op1, const Schedule& op2 )
{
    return ( op1.getDaysSinceDeparture () > op2.getDaysSinceDeparture () ||
             op1.getDaysSinceDeparture () == op2.getDaysSinceDeparture () &&
             op1.getHour () >= op2.getHour () );
}



bool
operator >= ( const Schedule& op1, const Hour& op2 )
{
    return ( op1.getHour () >= op2 );
}




bool
operator > ( const Schedule& op1, const Hour& op2 )
{
    return ( op1.getHour () > op2 );
}



int
operator - ( const Schedule& op1, const Schedule& op2 )
{
    int retain = 0;

    // 1: Hour
    int result = op1.getHour () - op2.getHour ();

    if ( result < 0 )
    {
        retain = 1;
        result += MINUTES_PER_DAY;
    }

    // 2: Days since departure
    result += ( op1.getDaysSinceDeparture () - op2.getDaysSinceDeparture () - retain )
              * MINUTES_PER_DAY;

    return result;
}




std::ostream&
operator << ( std::ostream& os, const Schedule& op )
{
    os << op.getDaysSinceDeparture ();
    os << op.getHour ();
    return os ;
}




Schedule&
Schedule::operator = ( const std::string& op )
{
    if ( op.size () == 0 )
    {
        _daysSinceDeparture = 0;
        _hour = op;
    }
    else if ( op[ 0 ] == TIME_MIN )
        setMinimum();
    else if ( op[ 0 ] == TIME_MAX )
        setMaximum();
    else
    {
        _daysSinceDeparture = atoi ( op.substr ( 0, 1 ).c_str () );
        _hour = op.substr( 1 );
    }
    return *this;
}



Schedule
Schedule::FromString (const std::string& str)
{
    // DD:hh:mm
    return Schedule (
	Hour (Conversion::ToInt (str.substr (3, 2)),
	      Conversion::ToInt (str.substr (6, 2))),
	Conversion::ToInt (str.substr (0, 2))
	);
}




}
}
