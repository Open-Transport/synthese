#include "Day.h"

#include "assert.h"


namespace synthese
{
namespace time
{

Day::Day ( DayValue value )
        : _value ( value )
{
    assert ( ( _value >= 1 ) && ( _value <= 31 ) );
}


Day::Day ( const Day& ref )
        : _value ( ref._value )
{
    assert ( ( _value >= 1 ) && ( _value <= 31 ) );
}


DayValue
Day::getValue () const
{
    return _value;
}




Day&
Day::operator= ( const DayValue value )
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
}


}
}
