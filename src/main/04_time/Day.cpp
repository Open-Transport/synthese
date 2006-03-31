#include "Day.h"

#include "assert.h"


namespace synthese
{
namespace time
{

Day::Day ( int value )
        : _value ( value )
{
    assert ( ( _value >= 1 ) && ( _value <= 31 ) );
}


Day::Day ( const Day& ref )
        : _value ( ref._value )
{
    assert ( ( _value >= 1 ) && ( _value <= 31 ) );
}


int
Day::getValue () const
{
    return _value;
}




Day&
Day::operator= ( const int value )
{
    _value = value;
    return *this;
}

Day&
Day::operator--( int )
{
    _value--;
    return *this;
}

Day&
Day::operator++( int )
{
    _value++;
    return *this;
}



std::ostream& operator<< ( std::ostream& os, const Day& op )
{
    os << op.getValue ();
    return os;
}


}
}
