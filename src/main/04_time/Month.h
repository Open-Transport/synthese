#ifndef SYNTHESE_TIME_MONTH_H
#define SYNTHESE_TIME_MONTH_H


#include "module.h"

#include <iostream>



namespace synthese
{
namespace time
{

class Year;


/** Month of year
@ingroup m04
*/
class Month
{
    private:
        int _value; //!< Numeric value for month (1=January ... 12=December)

    public:

        Month ( int value );
        Month ( const Month& ref );

        //! @name Getters/Setters
        //@{
        int getValue () const;
        //@}


        //! @name Query methods
        //@{

        /** Number of days in this month.
          \param year Year for this month
          \return Number of days in this month, taking into account leap years
        */
        int getDaysCount ( const Year& ) const;

        /** Number of days left til the end of this month.
          \param day Current day of this month
          \param year Year of this month
          \return Number of days left til the end of this month,
          excluding current day
        */
        int getDaysLeftToEndOfMonth ( int day, const Year& year ) const;


        int getDaysLeftToMonth ( const Year& startYear,
                                          int endMonth,
                                          const Year& endYear ) const;

        //@}


        Month& operator=( int );
        Month& operator--( int );
        Month& operator++( int );

};

std::ostream& operator<< ( std::ostream& os, const Month& op );



}
}



#endif
