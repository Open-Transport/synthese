#include "Year.h"




namespace synthese
{
namespace time
{


Year::Year ( int value )
        : _value ( value )
{}


Year::Year ( const Year& ref )
        : _value ( ref._value )
{}



int
Year::getValue () const
{
    return _value;
}



bool
Year::isLeapYear() const
{
    if ( _value % 400 == 0 )
        return true;
    if ( _value % 100 == 0 )
        return false;
    return ( _value % 4 == 0 );
}


int
Year::getDaysCount () const
{
    return isLeapYear () ? LEAP_YEAR_DAYS_COUNT :
           NON_LEAP_YEAR_DAYS_COUNT;
}






Year&
Year::operator=( int value )
{
    _value = value;
    return *this;
}


Year&
Year::operator--( int )
{
    _value--;
    return *this;
}


Year&
Year::operator++( int )
{
    _value++;
    return *this;
}



bool
operator<= ( const Year& op1, const Year& op2 )
{
    return op1.getValue() <= op2.getValue();
}



bool
operator== ( const Year& op1, const Year& op2 )
{
    return op1.getValue() == op2.getValue();
}



Year
operator-( const Year& op1, const Year& op2 )
{
    return Year ( op1.getValue() - op2.getValue() );
}


std::ostream&
operator<< ( std::ostream& os, const Year& op )
{
    os << op.getValue ();
    return os;
}



}
}
