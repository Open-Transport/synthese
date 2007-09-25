
/** DateTime class implementation.
	@file DateTime.cpp

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

#include "DateTime.h"

#include "04_time/TimeParseException.h"
#include "04_time/Schedule.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace time
	{
		DateTime::DateTime ( int day, int month, int year,
							int hours, int minutes )
							: _date(day, month, year)
							, _hour((hours == TIME_SAME) ? day : hours, minutes)
		{
		}




		DateTime::DateTime (const DateTime& ref)
			: _date(ref._date)
			, _hour(ref._hour)
		{
		}



		DateTime::DateTime( const Date& date )
			: _date(date)
			, _hour(TIME_MIN)
		{
		}

		DateTime::DateTime( const Date& date, const Hour& hour )
			: _date(date), _hour(hour)
		{	}

		DateTime::DateTime( const Date& date, const Schedule& schedule )
			: _date(date), _hour(schedule.getHour())
		{
			_date += schedule.getDaysSinceDeparture();
		}

		DateTime::~DateTime()
		{
		}



		DateTime& 
		DateTime::operator = ( const DateTime& ref)
		{
			if (this == &ref) return (*this);
			_date = ref._date;
			_hour = ref._hour;
			return (*this);
		}



		DateTime&
		DateTime::operator += ( int minutesDuration )
		{
			if (minutesDuration < 0)
				return operator -=(-minutesDuration);
			
			_date += ( _hour += minutesDuration );
			return ( *this );
		}





		DateTime&
		DateTime::operator -= ( int minutesDuration )
		{
			if (minutesDuration < 0)
				return operator +=(-minutesDuration);

			_date -= ( _hour -= minutesDuration );
			return ( *this );
		}



		DateTime&
		DateTime::operator ++( int )
		{
			_date++;
			return ( *this );
		}



		DateTime&
		DateTime::operator --( int )
		{
			_date--;
			return ( *this );
		}



		DateTime&
		DateTime::operator = ( const string& op )
		{
		_date = op.substr (0, 8);
		_hour = op.substr (8, 4);
		return (*this);

		}



		bool
		DateTime::isValid () const
		{
			return ( _date.isValid () && _hour.isValid () );
		}



		DateTime&
		DateTime::operator = ( const Date& op )
		{
			_date = op;
			return ( *this );
		}



		DateTime&
		DateTime::operator = ( const Hour& op )
		{
			_hour = op;
			return ( *this );
		}



		int
		DateTime::getDay () const
		{
			return _date.getDay ();
		}



		int
		DateTime::getMonth () const
		{
			return _date.getMonth ();
		}




		int
		DateTime::getYear () const
		{
			return _date.getYear ();
		}



		int
		DateTime::getMinutes() const
		{
			return _hour.getMinutes();
		}



		int DateTime::getHours () const
		{
			return _hour.getHours ();
		}



		const Date&
		DateTime::getDate() const
		{
			return _date;
		}



		const Hour&
		DateTime::getHour() const
		{
			return _hour;
		}


		void 
		DateTime::setHour ( const Hour& hour)
		{
		_hour = hour;
		}



		void
		DateTime::addDaysDuration ( int daysToAdd )
		{
			_date += daysToAdd;
		}



		void
		DateTime::subDaysDuration ( int daysToSub )
		{
			_date -= daysToSub;

		}



		string 
		DateTime::toInternalString () const
		{
			return string (getDate ().toInternalString() + 
					getHour ().toInternalString ());
		}




		ostream&
		operator<< ( ostream& os, const DateTime& op )
		{
			os << op.getDate () << op.getHour ();
			return os;
		}







		bool
			DateTime::operator == ( const DateTime& op2 ) const
		{
			return (getDate () == op2.getDate () ) &&
				( getHour () == op2.getHour () );
		}





		bool
			DateTime::operator != ( const DateTime& op2 ) const
		{
			return ( getDate () != op2.getDate () || getHour () != op2.getHour () );
		}



		bool
			DateTime::operator<=( const DateTime &op2 ) const
		{
			return ( getDate () < op2.getDate () || getDate () == op2.getDate () && getHour () <= op2.getHour () );
		}



		bool
			DateTime::operator < ( const DateTime &op2 ) const
		{
			return ( getDate () < op2.getDate () || getDate () == op2.getDate () && getHour () < op2.getHour () );
		}



		bool
			DateTime::operator>=( const DateTime& op2 ) const
		{
			return ( getDate () > op2.getDate () ||
					getDate () == op2.getDate () && getHour () >= op2.getHour () );
		}



		bool
			DateTime::operator > ( const DateTime &op2 ) const
		{
			return ( getDate () > op2.getDate () ||
					( getDate () == op2.getDate () && getHour () > op2.getHour () ) );
		}


		int 
		operator - ( const DateTime& op1, const DateTime& op2 )
		{
		int result;
		int retain = 0;
		  
		// 1: Hour
		result = op1.getHour () - op2.getHour ();
		if (result < 0)
			{
			retain = 1;
			result += MINUTES_PER_DAY;
			}
		  
		// 2: Days since departure
		result += ((op1.getDate () - op2.getDate ()) - retain) * MINUTES_PER_DAY;
		return result;
		}


		DateTime 
		operator + ( const DateTime& op, int minutesDuration )
		{
			DateTime result (op);
			result += minutesDuration;
			return result;
		}



		DateTime 
		operator - ( const DateTime& op, int minutesDuration )
		{
			DateTime result (op);
			result -= minutesDuration;
			return result;
		}




		DateTime // AAAA/(M)M/(J)J hh:mm
		DateTime::FromSQLTimestamp (const string& sqlTimestamp)
		{
			if (sqlTimestamp == "")
				return DateTime(time::TIME_UNKNOWN);

			int spaceSeparator = (int) sqlTimestamp.find(' ');

			return (spaceSeparator == -1)
				? DateTime(Date::FromSQLDate(sqlTimestamp))
				: DateTime(Date::FromSQLDate(sqlTimestamp.substr(0, spaceSeparator)), Hour::FromSQLTime(sqlTimestamp.substr(spaceSeparator+1, sqlTimestamp.length() - spaceSeparator)));
		}



		DateTime 
		DateTime::FromString (const string& str)
		{
			if (str.empty())
				return DateTime(time::TIME_UNKNOWN);

			int spaceSeparator = (int) str.find(' ');

			return (spaceSeparator == -1)
				? DateTime(Date::FromString(str))
				: DateTime(Date::FromString(str.substr(0, spaceSeparator)), Hour::FromSQLTime(str.substr(spaceSeparator+1, str.length() - spaceSeparator)));
		}

		string DateTime::toSQLString(bool withApostrophes) const
		{
			return isUnknown()
				? "NULL"
				: ((withApostrophes ? "'" : string()) + _date.toSQLString(false) + " " + _hour.toSQLString(false) + (withApostrophes ? "'" : string()));
		}

		bool DateTime::isUnknown() const
		{
			return _date.isUnknown();
		}

		string DateTime::toString() const
		{
			return isUnknown()
				? string()
				: (_date.toString() + " " + _hour.toString());
		}

		DateTime DateTime::FromInternalString( const string& str )
		{
			if(	str.size() == 1 
			&&	(	str.at(0) == TIME_CURRENT
				||	str.at(0) == TIME_TOMORROW
				)
			){
				return DateTime(str.at(0));
			}

			if (str.length() < 12)
				throw TimeParseException("Invalid string length");

			DateTime result(Conversion::ToInt (str.substr (6, 2)),
				Conversion::ToInt (str.substr (4, 2)),
				Conversion::ToInt (str.substr (0, 4)),
				Conversion::ToInt (str.substr (8, 2)),
				Conversion::ToInt (str.substr (10, 2))
			);

			if (!result.isValid())
				throw TimeParseException("Invalid date time value");

			return result;
		}
	}
}



