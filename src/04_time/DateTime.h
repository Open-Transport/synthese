////////////////////////////////////////////////////////////////////////////////
/// DateTime class header.
///	@file DateTime.h
///	@author Hugues Romain
///	@date 2008-12-26 21:19
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_TIME_DATETIME_H
#define SYNTHESE_TIME_DATETIME_H

#include "module.h"

#include "Date.h"
#include "Hour.h"

#include <iostream>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace time
	{
		////////////////////////////////////////////////////////////////////
		/// A date and a time.
		///	@ingroup m04
		class DateTime
		{
		public:
			typedef bool (DateTime::*ComparisonOperator) (const time::DateTime&) const;

		private:

			Date _date; //!< Date
			Hour _hour; //!< Hour

		public:

			DateTime (
				int day
				, int month = TIME_SAME
				, int year = TIME_SAME
				, int hours = TIME_SAME
				, int minutes = TIME_SAME
				, int seconds = TIME_SAME
			);

			DateTime ( const DateTime& ref);
			DateTime ( const Date& date );
			DateTime(const Date& date, const Hour& hour);
			DateTime(const Date& date, const Schedule& schedule);

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
				void subDaysDuration ( int daysDuration );
				void addDaysDuration ( int daysDuration = 1);
			//@}


			//! @name Query methods
			//@{
				bool isValid () const;
				bool isUnknown() const;
				virtual std::string toSQLString(bool withApostrophes = true) const;
				std::string toString(bool withSeconds = false) const;
				boost::posix_time::ptime toPosixTime() const;
			//@}



			/** Adds one day to this DateTime
			*/
			DateTime& operator ++ ( int );


			/** Subs one day to this DateTime
			*/
			DateTime& operator -- ( int );

			DateTime& operator += ( int minutesDuration );
			DateTime& operator -= ( int minutesDuration );

			DateTime& operator = ( const DateTime& ref);

			DateTime& operator = ( const Date& );
			DateTime& operator = ( const Hour& );


			/** Constructs a DateTime from an SQL timestamp string (AAAA-MM-JJ: hh:mm:ss);
				seconds are ignored.
				@param str Text to parse :
					- YYYY-MM-DD HH:MM : SQL date time
					- A/T/M/m/... internal command
			*/
			static DateTime FromSQLTimestamp (const std::string& sqlTimestamp);


			/** Constructs a DateTime from a string AAAA/MM/JJ hh:mm:ss;
				seconds are ignored.				
			*/
			static DateTime FromString (const std::string& str);

			bool operator == (const DateTime& op2 ) const;
			bool operator != (const DateTime& op2 ) const;
			bool operator <= (const DateTime& op2 ) const;
			bool operator < (const DateTime& op2 ) const;
			bool operator >= (const DateTime& op2 ) const;
			bool operator > (const DateTime& op2 ) const;

			/** Minutes duration calculation.
				@return The number of minutes between two DateTime objects.
			*/
			int operator - (const DateTime& op2 ) const;

			DateTime operator + (int minutesDuration ) const;
			DateTime operator - (int minutesDuration ) const;

			int getSecondsDifference(const DateTime& op2) const;
		};

		std::ostream& operator<< ( std::ostream& os, const DateTime& op );
	}
}

#endif
