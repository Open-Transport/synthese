#ifndef SYNTHESE_TIME_HOUR_H
#define SYNTHESE_TIME_HOUR_H


#include "module.h"

#include <iostream>
#include <string>



namespace synthese
{
namespace time
{

class Schedule;

/** Heure
@ingroup m04
*/
class Hour
{
        HourValue _hours; //!< Hours
        MinuteValue _minutes; //!< Minutes

    public :

        Hour ( HourValue hours, MinuteValue minutes );
        Hour ( const Hour& ref );


        ~Hour();

        //! \name Getters/Setters
        //@{
        MinuteValue getMinutes() const;
        HourValue getHours() const;
        //@}

        //! \name Update methods
        //@{

        /*! \brief Sets time including special characters conventions
          \param hour Hours (A = current, M=maximum, m=minimum, _=unchanged)
          \param minutes Minutes (A = current, M=maximum, m=minimum,
          _=unchanged, I=same than hours)
        */
        void setTimePattern( HourValue hour = TIME_CURRENT,
                             MinuteValue minutes = TIME_SAME );

        //@}



        /*! \brief Add minutes to this hour and return the number of
          day to add if needed (retain)
          \warning Only positive durations are handled!
        */
        DaysDuration operator += ( MinutesDuration );

        /*! \brief Substract minutes to this hour and return the number of
          day to substract if needed (retain)
          \warning Only positive durations are handled!
        */
        DaysDuration operator -= ( MinutesDuration );

        /*! \param op Updates this hour according to the meaning of
          the given 4 special characters ( m => minimum, M =>
          maximum...  max = 5 chars)
         */
        Hour& operator = ( const std::string& op );

        Hour& operator = ( const Schedule& op );

};


bool operator == ( const Hour& op1, const Hour& op2 );
bool operator != ( const Hour& op1, const Hour& op2 );
bool operator <= ( const Hour& op1, const Hour& op2 );
bool operator < ( const Hour& op1, const Hour& op2 );
bool operator >= ( const Hour& op1, const Hour& op2 );
bool operator > ( const Hour& op1, const Hour& op2 );

/*! \brief Number of minutes elapsed between two hours
*/
MinutesDuration operator- ( const Hour& op1, const Hour& op2 );

std::ostream& operator<< ( std::ostream& os, const Hour& op );





}
}

#endif
