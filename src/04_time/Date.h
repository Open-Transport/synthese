
/** Date class header.
	@file Date.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_TIME_DATE_H
#define SYNTHESE_TIME_DATE_H

#include "01_util/Constants.h"

#include "module.h"

#include <iostream>
#include <string>

#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace time
	{
		/** Date.
			@ingroup m04
		*/
		class Date
		{
		private:
			int _day; //!< Day
			int _month; //!< Month
			int _year; //!< Year
		    
		public:

			static const Date UNKNOWN_DATE;

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
			Date(
				int day
				, int month = TIME_SAME
				, int year = TIME_SAME
			);

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

				std::string getTextWeekDay() const;


				/** Checks this date values.
					@return true If this date is an existing day.
				*/
				bool isValid () const;

				bool isUnknown () const;

				std::string toSQLString(bool withApostrophes = true) const;
				std::string toString() const;

				
				/** Number of days in this month.
					@param year Year for this month
				  \return Number of days in this month, taking into account leap years
				*/
				int getDaysPerMonth() const;

				/** Number of days left til the end of this month.
				  \param day Current day of this month
				  \param year Year of this month
				  \return Number of days left til the end of this month,
				  excluding current day
				*/
				int getDaysLeftToEndOfMonth () const;


				/** Marc should comment his function.
					@param endMonth End month
					@param endYear End year
					@return what Marc wants to this function returns
				*/
				int getDaysLeftToMonth (int endMonth, int endYear ) const;


				bool isLeapYear () const;
				int getDaysPerYear () const;

				//@}


			/** Constructs a Date from an SQL date string or a one character internal command.
				@param sqlDate Text to parse :
					- YYYY-MM-DD : SQL date
					- A/M/m/T... : internal command, see @ref Date::Date "Date Constructor documentation"
				@throw TimeParseException if the text cannot be parsed
			*/
			static Date FromSQLDate (const std::string& sqlDate);


			static boost::optional<Date> FromSQLOptionalDate(
				const std::string& sqlDate
			);

			static Date FromString (const std::string& sqlString);



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

				int operator - ( const Date& op2 ) const;

				bool operator <  (const Date& op2 ) const;
				bool operator <= (const Date& op2 ) const;
				bool operator >  (const Date& op2 ) const;
				bool operator >= (const Date& op2 ) const;
				bool operator == (const Date& op2 ) const;
				bool operator != (const Date& op2 ) const;

				boost::gregorian::date toGregorianDate() const;
		};

		std::ostream& operator<< ( std::ostream& os, const Date& op );

	}
}

#endif
