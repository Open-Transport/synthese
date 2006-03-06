#ifndef SYNTHESE_TIME_DATETIME_H
#define SYNTHESE_TIME_DATETIME_H


#include "module.h"

#include "Date.h"
#include "Hour.h"


#include <iostream>
#include <string>



namespace synthese
{
namespace time
{


class Schedule;

/** DateTime
      @ingroup m04
*/
class DateTime
{
    private:

        Date _date; //!< Date
        Hour _hour; //!< Hour

    public:

        DateTime ( int day = TIME_CURRENT, int month = TIME_SAME, int year = TIME_SAME,
                   int hours = TIME_SAME, int minutes = TIME_SAME );

        DateTime ( const Date& date );

        ~DateTime();

        //! @name Getters/Setters
        //@{
        const Date& getDate() const;
        const Hour& getHour() const;

        int getYear () const;
        int getDay() const;
        int getMonth() const;
        int getHours() const;
        int getMinutes() const;

        void setHour ( const Hour& );
        //@}

        //! @name Update methods
        //@{
        void updateDateTime ( int day = TIME_CURRENT, int month = TIME_SAME,
                              int year = TIME_SAME, int hours = TIME_SAME,
                              int minutes = TIME_SAME );

        void updateDate ( int day = TIME_CURRENT, int month = TIME_SAME, int year = TIME_SAME );
        void updateHour ( int hours = TIME_SAME, int minutes = TIME_SAME );

        void updateHour ( const std::string& );
        void subDaysDuration ( int daysDuration );
        void addDaysDuration ( int daysDuration = 1);
        //@}


        //! @name Query methods
        //@{
        bool isValid () const;
        std::string toInternalString () const;
        //@}



        /** Adds one day to this DateTime
        */
        DateTime& operator ++ ( int );


        /** Subs one day to this DateTime
        */
        DateTime& operator -- ( int );

        DateTime& operator += ( int minutesDuration );
        DateTime& operator -= ( int minutesDuration );
        DateTime& operator = ( const std::string& );
        DateTime& operator = ( const Date& );
        DateTime& operator = ( const Hour& );


        /** Sets schedule hour to this DateTime.
        */
        DateTime& operator = ( const Schedule& );


};


std::ostream& operator<< ( std::ostream& os, const DateTime& op );


bool operator == ( const DateTime& op1, const DateTime& op2 );
bool operator != ( const DateTime& op1, const DateTime& op2 );
bool operator <= ( const DateTime& op1, const DateTime& op2 );
bool operator <= ( const DateTime& op1, const Schedule& op2 );
bool operator < ( const DateTime& op1, const DateTime& op2 );
bool operator < ( const DateTime& op1, const Schedule& op2 );
bool operator >= ( const DateTime& op1, const DateTime& op2 );
bool operator >= ( const DateTime& op1, const Schedule& op2 );
bool operator > ( const DateTime& op1, const DateTime& op2 );
bool operator > ( const DateTime& op1, const Schedule& op2 );

/**
 * @return The number of minutes between two DateTime objects.
 */
int operator - ( const DateTime& op1, const DateTime& op2 );


}
}

#endif
