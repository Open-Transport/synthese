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
        int _hours; //!< Hours
        int _minutes; //!< Minutes

    public :

        Hour ( int hours, int minutes );
        Hour ( const Hour& ref );


        ~Hour();

        //! @name Getters/Setters
        //@{
        int getMinutes() const;
        int getHours() const;
        //@}

        //! @name Update methods
        //@{

        /** Sets time including special characters conventions.
          @param hour Hours (A = current, M=maximum, m=minimum, _=unchanged)
          @param minutes Minutes (A=current, M=maximum, m=minimum,
                                  _=unchanged, I=same than hours)
        */
        void setTimePattern( int hour = TIME_CURRENT,
                             int minutes = TIME_SAME );

        //@}



        /** Add minutes to this hour and return the number of
          day to add if needed (retain).
          @warning Only positive durations are handled!
        */
        int operator += ( int );

        /** Substract minutes to this hour and return the number of
          day to substract if needed (retain)
          @warning Only positive durations are handled!
        */
        int operator -= ( int );

        /** @param op Updates this hour according to the meaning of
                      the given 4 special characters ( m => minimum, M =>
                      maximum...  max = 5 chars).
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

/** Number of minutes elapsed between two hours
*/
int operator- ( const Hour& op1, const Hour& op2 );

std::ostream& operator<< ( std::ostream& os, const Hour& op );





}
}

#endif
