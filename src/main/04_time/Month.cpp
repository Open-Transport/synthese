
#include "Month.h"
#include "Year.h"

#include "assert.h"

#include <iomanip>


namespace synthese
{
namespace time
{




Month::Month ( int value )
        : _value ( value )
{
    assert ( ( _value >= 1 ) && ( _value <= 12 ) );
}


Month::Month ( const Month& ref )
        : _value ( ref._value )
{
    assert ( ( _value >= 1 ) && ( _value <= 12 ) );
}




int
Month::getValue () const
{
    return _value;
}




int
Month::getDaysCount ( const Year& year ) const
{
    switch ( _value )
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            return year.isLeapYear () ? 29 : 28;
    }

    return 0;
}



int
Month::getDaysLeftToEndOfMonth ( int day, const Year& year ) const
{
    return ( getDaysCount ( year ) - day );
}



Month&
Month::operator=( int value )
{
    _value = value;
    return *this;
}



Month&
Month::operator--( int )
{
    _value--;
    return *this;
}



Month&
Month::operator++( int )
{
    _value++;
    return *this;
}




int
Month::getDaysLeftToMonth ( const Year& startYear,
                            int endMonth,
                            const Year& endYear ) const
{
    int result = 0;
    Month startMonth ( *this );

    Year tmpStartYear ( startYear );

    while ( tmpStartYear.getValue() < endYear.getValue() ||
            startMonth.getValue () < ( endMonth - 1 ) )
    {
        startMonth++;
        if ( startMonth.getValue () > MONTHS_PER_YEAR )
        {
            startMonth = 1;
            tmpStartYear++;
        }

        result = result + startMonth.getDaysCount ( tmpStartYear );
    }

    return result;
}




std::ostream&
operator<< ( std::ostream& os, const Month& op )
{
    os << std::setw( 2 ) << std::setfill ( '0' )
    << op.getValue ();
    return os;
}




















}
}

