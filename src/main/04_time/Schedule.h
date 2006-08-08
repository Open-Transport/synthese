#ifndef SYNTHESE_TIME_SCHEDULE_H
#define SYNTHESE_TIME_SCHEDULE_H


#include "module.h"
#include "Hour.h"

#include <iostream>
#include <string>


namespace synthese
{
namespace time
{

/** Schedule (hour + days duration since departure)
@ingroup m04
*/
class Schedule
{
        Hour _hour; //!< Hour
        int _daysSinceDeparture; //!< Elapsed days since departure (relative)

    public:

        Schedule ();
        Schedule ( const Hour& hour, int daysSinceDeparture );
        Schedule ( const Schedule& ref );
        ~Schedule ();


        //! @name Getters/Setters
        //@{
        const Hour& getHour() const;
        int getMinutes() const;
        int getHours() const;

        int getDaysSinceDeparture () const;
        void setDaysSinceDeparture ( int daysSinceDeparture );

        //@}


        //! @name Query methods
        //@{
	bool isValid () const;
	//@}


        //! @name Update methods
        //@{
        void setMinimum();
        void setMaximum();
        //@}


        /** Updates this schedule according to the meaning of
          the given special characters ( m => minimum, M =>
          maximum... max = 5 chars).
         */
        Schedule& operator = ( const std::string& );

        /** Adds a number of minutes to this schedule.
          \param op Number of minutes to add
          \warning Only positive durations!
        */
        Schedule& operator += ( int op );

	static Schedule Schedule::FromString (const std::string& str);

};

bool operator < ( const Schedule& op1, const Schedule& op2 );
bool operator < ( const Schedule& op1, const Hour& op2 );
bool operator <= ( const Schedule& op1, const Schedule& op2 );
bool operator <= ( const Schedule& op1, const Hour& op2 );
bool operator >= ( const Schedule& op1, const Schedule& op2 );
bool operator >= ( const Schedule& op1, const Hour& op2 );
bool operator > ( const Schedule& op1, const Hour& op2 );

/** Returns number of minutes elapsed between two
schedules. If this \< op2, the returned duration is negative.
*/
int operator - ( const Schedule& op1, const Schedule& op2 );

std::ostream& operator<< ( std::ostream& os, const Schedule& op );


}

}
#endif
