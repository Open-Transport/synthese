#ifndef SYNTHESE_TIME_YEAR_H
#define SYNTHESE_TIME_YEAR_H


#include "module.h"

#include <iostream>



namespace synthese
{
namespace time
{



/** Year class
    @ingroup m04
*/
class Year
{
    private:

        YearValue _value; //!< Numeric value for year

    public:

        Year ( YearValue value );
        Year ( const Year& );


        //! \name Getters/Setters
        //@{
        YearValue getValue () const;
        //@}


        //! \name Query methods
        //@{
        bool isLeapYear () const;
        DaysDuration getDaysCount () const;
        //@}


        Year& operator= ( YearValue value );
        Year& operator-- ( int );
        Year& operator++ ( int );



};


bool operator<= ( const Year& op1, const Year& op2 );
bool operator== ( const Year& op1, const Year& op2 );
Year operator- ( const Year& op1, const Year& op2 );
std::ostream& operator<< ( std::ostream& os, const Year& op );


}

}

#endif




