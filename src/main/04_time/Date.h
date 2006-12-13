#ifndef SYNTHESE_TIME_DATE_H
#define SYNTHESE_TIME_DATE_H


#include "module.h"

#include "Day.h"
#include "Month.h"
#include "Year.h"


#include <iostream>
#include <string>



namespace synthese
{
namespace time
{

class DateTime;

/** Date
@ingroup m04
*/
class Date
{
 private:


    Day _day; //!< Day
    Month _month; //!< Month
    Year _year; //!< Year
    
 public:

    static const Date UNKNOWN_DATE;

        Date( int day = UNKNOWN_VALUE,
              int month = UNKNOWN_VALUE,
              int year = UNKNOWN_VALUE );

        ~Date();

        //! @name Getters/Setters
        //@{
        int getDay() const;
        int getMonth() const;
        int getYear() const;
        //@}


        //! @name Query methods
        //@{

        /** Gets week day of this date.
            @return 0 = Sunday, 1 = Monday, ... , 6 = Saturday
        */
        int getWeekDay () const;

        bool isYearUnknown () const;

        /** Checks this date values.
            @return true If this date is an existing day.
        */
        bool isValid () const;

        bool isUnknown () const;

        std::string toInternalString () const;
		std::string toSQLiteString(bool withApostrophes = true) const;

        //@}


	/** Constructs a Date from an SQL date string (AAAA-MM-JJ).
		@todo Throw an exception on parsing error
	*/
	static Date FromSQLDate (const std::string& sqlDate);

	static Date FromString (const std::string& sqlString);

        //! @name Update methods
        //@{
        /** Update this date given three int values, without control.
            
			@param day Day value
            @param month Month value
            @param year Year value

            Allowed values for each param :
				- int number : integral value "as is" without control
				(31/2/1650 is acceptable).
				- TIME_CURRENT ('A') = identical to system current time.
				- TIME_MAX ('M') = identical to max absolute date 31/12/9999
				- TIME_MIN ('m') = identical to min absolute date 1/1/1
				- TIME_SAME ('I') = identical to previous special char ?
				- TIME_UNKNOWN ('?') = unknown date (codée -1/-1/-1)
				- TIME_UNCHANGED ('_') = no modification

            Any invalid special character is taken as TIME_UNCHANGED

            Main uses :
				- use without param : current date
				- use with special char : date set according so command 
					character (ex : updateDate(TIME_MAX) => 31/12/9999)
				- use with three values : fixed date (ex : updateDate(2, 10, 2006) => 2/10/2006

            - use with three mixed params : special commands (ex :
            updateDate(15, TIME_CURRENT, TIME_CURRENT) => the 15th
            of current month)

            Special command chars follows current relantionship : 
            \f$ TIME_{UNKNOWN} \leq TIME_{MIN} \leq TIME_{CURRENT} \leq TIME_{MAX} \f$
        */
        void updateDate( int day = TIME_CURRENT, int month = TIME_SAME,
                         int year = TIME_SAME );

        //@}


        /** Adds one day to this date.
        */
        Date& operator ++ ( int );

        /** Subs one day to this date.
        */
        Date& operator -- ( int );

        /** Adds n days to this date.
        */
        Date& operator += ( int daysToAdd );

        /** Subs n days to this date.
        */
        Date& operator -= ( int daysToSub );


        /** Modifies this date from parsing a string.
            @param op String containing the new date to set
            (internal encoding)

            - if op is empty (-1/-1/-1)
            - if op is a special char string, it is interpreted
            (\ref Date::updateDate() )
            - if op is an internally encoded string (8 chars) then
              the date is modified subsequently
        */
        Date& operator = ( const std::string& op );

        Date& operator = ( const DateTime& op );

        int operator - ( const Date& op2 ) const;


};


bool operator < ( const Date& op1, const Date& op2 );
bool operator <= ( const Date& op1, const Date& op2 );
bool operator <= ( const Date& op1, const DateTime& op2 );
bool operator > ( const Date& op1, const Date& op2 );
bool operator >= ( const Date& op1, const Date& op2 );
bool operator == ( const Date& op1, const Date& op2 );
bool operator != ( const Date& op1, const Date& op2 );


std::ostream& operator<< ( std::ostream& os, const Date& op );


}
}

#endif

