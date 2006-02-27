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
        MonthValue _value; //!< Numeric value for month (1=January ... 12=December)

    public:

        Month ( MonthValue value );
        Month ( const Month& ref );

        //! \name Getters/Setters
        //@{
        MonthValue getValue () const;
        //@}


        //! \name Query methods
        //@{

        /*! \brief Number of days in this month
          \param year Year for this month
          \return Number of days in this month, taking into account leap years
        */
        DaysDuration getDaysCount ( const Year& ) const;

        /*! \brief Number of days left til the end of this month
          \param day Current day of this month
          \param year Year of this month
          \return Number of days left til the end of this month,
          excluding current day
        */
        DaysDuration getDaysLeftToEndOfMonth ( DayValue day, const Year& year ) const;


        DaysDuration getDaysLeftToMonth ( const Year& startYear,
                                          MonthValue endMonth,
                                          const Year& endYear ) const;

        //@}


        Month& operator=( MonthValue );
        Month& operator--( int );
        Month& operator++( int );

};

std::ostream& operator<< ( std::ostream& os, const Month& op );



}
}



#endif
