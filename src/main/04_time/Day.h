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

        int _value; //!< Numeric value for day (1 .. 31)

    public:

        Day ( int value );
        Day ( const Day& ref );


        //! @name Getters/Setters
        //@{
        int getValue () const;
        //@}

        Day& operator=( const int );
        Day& operator--( int );
        Day& operator++( int );

};

std::ostream& operator<< ( std::ostream& os, const Day& op );



}

}
#endif

