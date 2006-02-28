#include "DateTime.h"
#include "Schedule.h"


namespace synthese
{
namespace time
{




DateTime::DateTime ( int day, int month, int year,
                     int hours, int minutes )
{
    updateDateTime ( day, month, year, hours, minutes );
}


DateTime::DateTime( const Date& date )
{
    operator= ( date );
}



DateTime::~DateTime()
{
}




DateTime&
DateTime::operator += ( int minutesDuration )
{
    _date += ( _hour += minutesDuration );
    return ( *this );
}

DateTime&
DateTime::operator -= ( int minutesDuration )
{
    _date -= ( _hour -= minutesDuration );
    return ( *this );
}



DateTime&
DateTime::operator ++( int )
{
    _date++;
    return ( *this );
}



DateTime&
DateTime::operator --( int )
{
    _date--;
    return ( *this );
}



DateTime&
DateTime::operator = ( const Schedule& op )
{
    _hour = op.getHour ();
    return ( *this );
}



bool
DateTime::isValid () const
{
    return ( _date.isValid () && _hour.isValid () );
}



DateTime&
DateTime::operator = ( const Date& op )
{
    _date = op;
    return ( *this );
}



DateTime&
DateTime::operator = ( const Hour& op )
{
    _hour = op;
    return ( *this );
}



int
DateTime::getDay () const
{
    return _date.getDay ();
}



int
DateTime::getMonth () const
{
    return _date.getMonth ();
}




int
DateTime::getYear () const
{
    return _date.getYear ();
}



int
DateTime::getMinutes() const
{
    return _hour.getMinutes();
}



int DateTime::getHours () const
{
    return _hour.getHours ();
}



const Date&
DateTime::getDate() const
{
    return _date;
}



void
DateTime::updateDate( int day, int month, int year )
{
    _date.updateDate( day, month, year );
}




const Hour&
DateTime::getHour() const
{
    return _hour;
}




void
DateTime::updateHour ( int hours, int minutes )
{
    _hour.updateHour ( hours, minutes );
}




void
DateTime::addDaysDuration ( int daysToAdd )
{
    _date += daysToAdd;
}



void
DateTime::subDaysDuration ( int daysToSub )
{
    _date -= daysToSub;

}



void
DateTime::updateDateTime ( int day, int month, int year, int hours, int minutes )
{
    _date.updateDate ( day, month, year );
    if ( hours == TIME_SAME )
        _hour.updateHour ( day, minutes ); // is it right ??
    else
        _hour.updateHour ( hours, minutes );

}




std::ostream&
operator<< ( std::ostream& os, const DateTime& op )
{
    os << op.getDate () << op.getHour ();
    return os;
}







bool
operator == ( const DateTime& op1, const DateTime& op2 )
{
    return ( op1.getDate () == op2.getDate () ) &&
           ( op1.getHour () == op2.getHour () );
}





bool
operator != ( const DateTime& op1, const DateTime& op2 )
{
    return ( op1.getDate () != op2.getDate () || op1.getHour () != op2.getHour () );
}



bool
operator<=( const DateTime& op1, const DateTime &op2 )
{
    return ( op1.getDate () < op2.getDate () || op1.getDate () == op2.getDate () && op1.getHour () <= op2.getHour () );
}



bool
operator < ( const DateTime& op1, const DateTime &op2 )
{
    return ( op1.getDate () < op2.getDate () || op1.getDate () == op2.getDate () && op1.getHour () < op2.getHour () );
}



bool
operator<=( const DateTime& op1, const Schedule& op2 )
{
    return ( op2 >= op1.getHour () );
}



bool
operator<( const DateTime& op1, const Schedule& op2 )
{
    return ( op2 > op1.getHour () );
}



bool
operator>=( const DateTime& op1, const Schedule& op2 )
{
    return ( op2 <= op1.getHour () );
}



bool
operator>( const DateTime& op1, const Schedule& op2 )
{
    return ( op2 < op1.getHour () );
}



bool
operator>=( const DateTime& op1, const DateTime& op2 )
{
    return ( op1.getDate () > op2.getDate () ||
             op1.getDate () == op2.getDate () && op1.getHour () >= op2.getHour () );
}



bool
operator > ( const DateTime& op1, const DateTime &op2 )
{
    return ( op1.getDate () > op2.getDate () ||
             ( op1.getDate () == op2.getDate () && op1.getHour () > op2.getHour () ) );
}





}
}


