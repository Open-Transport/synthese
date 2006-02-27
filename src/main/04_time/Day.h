#ifndef SYNTHESE_TIME_DAY_H
#define SYNTHESE_TIME_DAY_H


#include "module.h"

#include <iostream>



namespace synthese
{
namespace time
{

/** Day
@ingroup m04
*/
class Day
{
    private:

        DayValue _value; //!< Numeric value for day (1 .. 31)

    public:

        Day ( DayValue value );
        Day ( const Day& ref );


        //! \name Getters/Setters
        //@{
        DayValue getValue () const;
        //@}

        Day& operator=( const DayValue );
        Day& operator--( int );
        Day& operator++( int );

};

std::ostream& operator<< ( std::ostream& os, const Day& op );



}

}
#endif
