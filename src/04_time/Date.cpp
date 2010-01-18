
/** Date class implementation.
	@file Date.cpp

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

#include <sstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <assert.h>

#include "Conversion.h"

#include "Date.h"
#include "DateTime.h"
#include "TimeParseException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace time
	{

		const Date Date::UNKNOWN_DATE(TIME_UNKNOWN);


		Date::Date( int day, int month, int year )
			: _day(UNKNOWN_VALUE), _month(UNKNOWN_VALUE), _year(UNKNOWN_VALUE)
		{
			time_t rawtime;
			struct tm * timeinfo = 0;

			bool tomorrow(false);
			if (day == TIME_TOMORROW)
			{
				day = TIME_CURRENT;
				tomorrow = true;
			}
			if (month == TIME_TOMORROW)
				month = TIME_CURRENT;
			if (year == TIME_TOMORROW)
				year = TIME_CURRENT;

			if ( day == TIME_CURRENT || month == TIME_CURRENT || year == TIME_CURRENT )
			{
				std::time ( &rawtime );
				timeinfo = localtime ( &rawtime );
			}

			// Month
			if ( month == TIME_CURRENT || month == TIME_SAME && day == TIME_CURRENT )
				_month = ( *timeinfo ).tm_mon + 1;
			else if ( month == TIME_MAX || month == TIME_SAME && day == TIME_MAX )
				_month = MONTHS_PER_YEAR;
			else if ( month == TIME_MIN || month == TIME_SAME && day == TIME_MIN )
				_month = 1;
			else if ( month == TIME_UNKNOWN || month == TIME_SAME && day == TIME_UNKNOWN )
				_month = UNKNOWN_VALUE;
			else if ( month != TIME_UNCHANGED &&
					( month != TIME_SAME || day != TIME_UNCHANGED ) &&
					month >= 1 && month <= MONTHS_PER_YEAR )
				_month = month;

			// Year
			if ( year == TIME_CURRENT || year == TIME_SAME && day == TIME_CURRENT )
				_year = ( *timeinfo ).tm_year + 1900;
			else if ( year == TIME_MAX || year == TIME_SAME && day == TIME_MAX )
				_year = MAX_YEAR;
			else if ( year == TIME_MIN || year == TIME_SAME && day == TIME_MIN )
				_year = 1;
			else if ( year == TIME_UNKNOWN || year == TIME_SAME && day == TIME_UNKNOWN )
				_year = UNKNOWN_VALUE;
			else if ( year != TIME_UNCHANGED && ( year != TIME_SAME || day != TIME_UNCHANGED ) && year >= 0 && year <= MAX_YEAR )
				_year = year;

			// Day
			if ( day == TIME_CURRENT )
				_day = ( *timeinfo ).tm_mday;
			else if ( day == TIME_MAX )
				_day = getDaysPerMonth();
			else if ( day == TIME_MIN )
				_day = 1;
			else if ( day == TIME_UNKNOWN )
				_day = UNKNOWN_VALUE;
			else if ( day != TIME_UNCHANGED && day >= 1 && day <= 31 )
				_day = day;

			if (tomorrow)
				operator++(0);
		}


		Date::~Date()
		{
		}



		int
		Date::getDay() const
		{
			return _day;
		}




		int
		Date::getMonth() const
		{
			return _month;
		}



		int
		Date::getYear() const
		{
			return _year;
		}



		bool
		Date::isValid () const
		{
			return _year >= 0
				&& _month > 0
				&& _month <= MONTHS_PER_YEAR
				&& _day > 0
				&& _day <= getDaysPerMonth();
		}




		int
		Date::getWeekDay () const
		{
			int mz = _month - 2;
			int az = _year;
			if ( mz <= 0 )
			{
				mz += MONTHS_PER_YEAR;
				az --;
			}
			int s = az / 100;
			int e = az % 100;

			int j = _day + static_cast<int>(floor ( 2.6 * mz - 0.2 ));
			j += e + ( e / 4 ) + ( s / 4 ) - 2 * s;
			if ( j >= 0 )
				j %= 7;
			else
			{
				j = ( -j ) % 7;
				if ( j > 0 )
					j = 7 - j;
			}
			return j;
		}



		bool
		Date::isUnknown () const
		{
			return _year == UNKNOWN_VALUE ||
				_month == UNKNOWN_VALUE ||
				_day == UNKNOWN_VALUE;
		}



		Date&
		Date::operator++( int )
		{
			if (!isUnknown())
			{
				_day++;
				if (_day > getDaysPerMonth() )
				{
					_day = 1;
					_month++;
					if (_month > MONTHS_PER_YEAR )
					{
						_month = 1;
						_year++;
					}
				}
			}
			return ( *this );
		}




		Date&
		Date::operator--( int )
		{
			if (!isUnknown())
			{
				_day--;
				if (_day == 0 )
				{
					_month--;
					if (_month == 0 )
					{
						_year--;
						_month = 12;
					}
					_day = getDaysPerMonth();
				}
			}
			return *this;
		}



		Date&
		Date::operator+=( int daysToAdd )
		{
			if (!isUnknown())
			{
				for ( ; daysToAdd != 0; daysToAdd-- )
					operator++( 0 );
			}
			return ( *this );
		}


		Date&
		Date::operator-=( int daysToSub )
		{
			if (!isUnknown())
			{
				for ( ; daysToSub != 0; daysToSub-- )
					operator--( 0 );
			}
			return ( *this );
		}



		Date& Date::operator = ( const std::string& op )
		{
			switch ( op.size() )
			{
				case 0:
					operator=(Date(TIME_UNKNOWN));
					break;

				case 1:
					operator=(Date(op[ 0 ]));
					break;

				default:
					int year = atoi ( op.substr ( 0, 4 ).c_str () );
					int month = atoi ( op.substr ( 4, 2 ).c_str () );
					int day = atoi ( op.substr ( 6, 2 ).c_str () );
					operator=(Date(day, month, year));
					break;
			}
			return *this;
		}



		std::ostream&		operator<< ( std::ostream& os, const Date& op )
		{
			os << op.toSQLString(false);
			return os;
		}



		bool Date::operator < (const Date& op2 ) const
		{
			return _year < op2._year
				|| _year == op2._year
				&& ( _month < op2._month
						|| _month == op2._month
						&& _day < op2._day
					);
		}


		bool Date::operator <= (const Date& op2 ) const
		{
			return _year < op2._year
				|| _year == op2._year
				&& ( _month < op2._month
						|| _month == op2._month
						&& _day <= op2._day
					);

		}



		bool Date::operator > (const Date& op2 ) const
		{
			return _year > op2._year
				|| _year == op2._year
				&& ( _month > op2._month
						|| _month == op2._month
						&& _day > op2._day
					);
		}


		bool Date::operator >= (const Date& op2 ) const
		{
			return _year > op2._year
				|| _year == op2._year
				&& ( _month > op2._month
						|| _month == op2._month
						&& _day >= op2._day
					);
		}


		bool Date::operator == (const Date& op2 ) const
		{
			return _year == op2._year
				&& _month == op2._month
				&& _day == op2._day
				;
		}


		bool Date::operator != (const Date& op2 ) const
		{
			return _year != op2._year
				|| _month != op2._month
				|| _day != op2._day
				;
		}




		int	Date::operator - ( const Date& op2 ) const
		{
			if ( *this < op2 )
				return ( -( op2 - *this ) );

			// Temporary...
			if (_month != op2._month || _year != op2._year)
			{
				return getDaysLeftToEndOfMonth()
					+ op2.getDaysLeftToMonth (_month, _year )
					+ op2._day;
			}
			else
			{
				return _day - op2._day;
			}
		}



		Date 
		Date::FromSQLDate (const std::string& sqlTimestamp)
		{
			if (sqlTimestamp.empty())
				return UNKNOWN_DATE;

			if(	sqlTimestamp.size() == 1)
				return Date(sqlTimestamp[0]);

			int firstSlash = (int) sqlTimestamp.find('-');
			int secondSlash = (int) sqlTimestamp.find('-', firstSlash+1);

			if (firstSlash == -1 || secondSlash == -1)
				throw TimeParseException(sqlTimestamp);

			return Date(Conversion::ToInt (sqlTimestamp.substr (secondSlash+1, sqlTimestamp.length() - secondSlash)),
				Conversion::ToInt (sqlTimestamp.substr (firstSlash+1, secondSlash - firstSlash)),
				Conversion::ToInt (sqlTimestamp.substr (0, firstSlash+1)));
		}


		Date 
		Date::FromString (const std::string& str)
		{
			if (str.empty())
				return UNKNOWN_DATE;

			int firstSlash = (int) str.find('/');
			int secondSlash = (int) str.find('/', firstSlash+1);

			if (firstSlash == -1 || secondSlash == -1)
				throw TimeParseException("Invalid date");

			return Date(
				Conversion::ToInt (str.substr (0, firstSlash+1)),
				Conversion::ToInt (str.substr (firstSlash+1, secondSlash - firstSlash)),
				Conversion::ToInt (str.substr (secondSlash+1, str.length() - secondSlash))
				);
		}

		std::string Date::toSQLString( bool withApostrophes ) const
		{
			stringstream s;
			if (withApostrophes)
				s << "'";
			if (!isUnknown())
			{
				s	<< setw( 4 ) << setfill ( '0' )
					<< _year << "-"
					<< setw( 2 ) << setfill ( '0' )
					<< _month << "-"
					<< setw( 2 ) << setfill ( '0' )
					<< _day;

			}
			if (withApostrophes)
				s << "'";
			return s.str();
		}

		std::string Date::toString() const
		{
			if (isUnknown())
				return string();

			return Conversion::ToString(_day) + "/" + Conversion::ToString(_month) +"/" + Conversion::ToString(_year);
		}



		int Date::getDaysLeftToMonth( int endMonth, int endYear ) const
		{
			int result = 0;
			Date startDate(1,_month,_year);
			
			while (startDate._year < endYear || startDate._month < ( endMonth - 1 ) )
			{
				++startDate._month;
				if (startDate._month > MONTHS_PER_YEAR )
				{
					startDate._month = 1;
					++startDate._year;
				}

				result = result + startDate.getDaysPerMonth();
			}

			return result;
		}



		int Date::getDaysLeftToEndOfMonth() const
		{
			return (getDaysPerYear() - _day );
		}



		int Date::getDaysPerYear() const
		{
			return isLeapYear() ? LEAP_YEAR_DAYS_COUNT :
				NON_LEAP_YEAR_DAYS_COUNT;
		}



		bool Date::isLeapYear() const
		{
			if ( _year % 400 == 0 )
				return true;
			if ( _year % 100 == 0 )
				return false;
			return _year % 4 == 0;
		}



		int Date::getDaysPerMonth() const
		{
			switch (_month)
			{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				return 31;
			case 4:
			case 6:
			case 9:
			case 11:
				return 30;
			case 2:
				return isLeapYear() ? 29 : 28;
			}

			assert(false);
			return 0;
		}



		boost::optional<Date> Date::FromSQLOptionalDate( const std::string& sqlTimestamp)
		{
			if (sqlTimestamp.empty())
				return optional<Date>();

			if(	sqlTimestamp.size() == 1)
			{
				if (sqlTimestamp[0] == TIME_UNKNOWN)
					return optional<Date>();
				else
					return Date(sqlTimestamp[0]);
			}

			int firstSlash = (int) sqlTimestamp.find('-');
			int secondSlash = (int) sqlTimestamp.find('-', firstSlash+1);

			if (firstSlash == -1 || secondSlash == -1)
				return optional<Date>();

			return Date(Conversion::ToInt (sqlTimestamp.substr (secondSlash+1, sqlTimestamp.length() - secondSlash)),
				Conversion::ToInt (sqlTimestamp.substr (firstSlash+1, secondSlash - firstSlash)),
				Conversion::ToInt (sqlTimestamp.substr (0, firstSlash+1)));
		}



		std::string Date::getTextWeekDay() const
		{
			int weekDay(getWeekDay());
			if(weekDay < 0 || weekDay > 6) return string();

			static vector<string> weekDays;
			weekDays.push_back("Dimanche");
			weekDays.push_back("Lundi");
			weekDays.push_back("Mardi");
			weekDays.push_back("Mercredi");
			weekDays.push_back("Jeudi");
			weekDays.push_back("Vendredi");
			weekDays.push_back("Samedi");


			return weekDays[weekDay];
		}



		boost::gregorian::date Date::toGregorianDate() const
		{
			return isUnknown() ? gregorian::date() : gregorian::date(_year, _month, _day);
		}



		synthese::time::Date Date::FromGregorianDate( boost::gregorian::date value )
		{
			if(value.is_not_a_date()) return Date(TIME_UNKNOWN);

			return Date(value.day(), value.month(), value.year());
		}
	}
}
