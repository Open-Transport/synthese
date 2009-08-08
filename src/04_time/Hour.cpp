
/** Hour class implementation.
	@file Hour.cpp

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

#include "Hour.h"

#include "assert.h"

#include <iomanip>
#include <sstream>
#include <ctime>

#include <boost/tokenizer.hpp>

#include "01_util/Conversion.h"

#include "04_time/Schedule.h"
#include "04_time/TimeParseException.h"

using namespace boost;

namespace synthese
{
	using namespace util;

	namespace time
	{

		Hour::Hour ( int hours, int minutes, int seconds )
				: _hours (UNKNOWN_VALUE)
				, _minutes (UNKNOWN_VALUE)
				, _seconds(0)
		{
			time_t rawtime;
			struct tm * timeinfo = 0;

			if (hours == TIME_TOMORROW)
				hours = TIME_CURRENT;
			if (minutes == TIME_TOMORROW)
				minutes = TIME_CURRENT;

			if ( hours == TIME_CURRENT || minutes == TIME_CURRENT )
			{
				std::time ( &rawtime );
				timeinfo = localtime ( &rawtime );
			}

			if ( hours == TIME_CURRENT )
				_hours = ( *timeinfo ).tm_hour;
			else if ( hours == TIME_MAX )
				_hours = 23;
			else if ( hours == TIME_MIN )
				_hours = 0;
			else if ( hours == TIME_UNKNOWN )
				_hours = UNKNOWN_VALUE;
			else if ( hours != TIME_UNCHANGED )
				_hours = hours;

			if ( minutes == TIME_CURRENT || minutes == TIME_SAME && hours == TIME_CURRENT )
				_minutes = ( *timeinfo ).tm_min;
			else if ( minutes == TIME_MAX || minutes == TIME_SAME && hours == TIME_MAX )
				_minutes = 59;
			else if ( minutes == TIME_MIN || minutes == TIME_SAME && hours == TIME_MIN )
				_minutes = 0;
			else if ( _minutes == TIME_UNKNOWN || minutes == TIME_SAME && hours == TIME_UNKNOWN )
				_minutes = UNKNOWN_VALUE;
			else if ( minutes != TIME_UNCHANGED && minutes != TIME_SAME)
				_minutes = minutes;

			if ( seconds == TIME_CURRENT || seconds == TIME_SAME && hours == TIME_CURRENT )
				_seconds = ( *timeinfo ).tm_sec;
			else if ( seconds == TIME_MAX || seconds == TIME_SAME && hours == TIME_MAX )
				_seconds = 59;
			else if ( seconds == TIME_MIN || seconds == TIME_SAME && hours == TIME_MIN )
				_seconds = 0;
			else if ( _seconds == TIME_UNKNOWN || seconds == TIME_SAME && hours == TIME_UNKNOWN )
				_seconds = UNKNOWN_VALUE;
			else if ( seconds != TIME_UNCHANGED && seconds != TIME_SAME)
				_seconds = seconds;
		}


		Hour::Hour ( const Hour& ref )
				: _hours ( ref._hours )
				, _minutes ( ref._minutes )
				, _seconds(ref._seconds)
		{
		}


		Hour::~Hour()
		{}




		bool
		Hour::isValid () const
		{
			return _hours >= 0 && _hours < HOURS_PER_DAY
				&& _minutes >= 0 && _minutes < MINUTES_PER_HOUR
				&& _seconds >= 0 && _seconds < SECONDS_PER_MINUTE
			;
		}



		bool
		Hour::isUnknown () const
		{
			return _hours == UNKNOWN_VALUE || _minutes == UNKNOWN_VALUE || _seconds == UNKNOWN_VALUE;
		}


		std::string Hour::toString (bool withSeconds) const
		{
			if (isUnknown())
				return "";

			std::stringstream os;
			os << std::setw( 2 ) << std::setfill ( '0' )
				<< _hours << ":" 
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< _minutes;
			if (withSeconds)
			{
			os << ":" 
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< _seconds;
			}
			return os.str ();
		}


		bool Hour::operator == (const Hour& op2 ) const
		{
			return _hours == op2._hours
				&& _minutes == op2._minutes
				&& _seconds == op2._seconds
			;
		}



		bool Hour::operator != (const Hour& op2 ) const
		{
			return _hours != op2._hours
				|| _minutes != op2._minutes
				|| _seconds != op2._seconds
			;
		}



		bool Hour::operator <= (const Hour& op2 ) const
		{
			if (_hours == op2._hours)
			{
				if (_minutes == op2._minutes)
					return _seconds <= op2._seconds;
				else
					return _minutes < op2._minutes;
			}
			return _hours < op2._hours;
		}



		bool Hour::operator<(const Hour& op2 ) const
		{
			if (_hours == op2._hours)
			{
				if (_minutes == op2._minutes)
					return _seconds < op2._seconds;
				else
					return _minutes < op2._minutes;
			}
			return _hours < op2._hours;
		}



		bool Hour::operator>=(const Hour& op2 ) const
		{
			if (_hours == op2._hours)
			{
				if (_minutes == op2._minutes)
					return _seconds >= op2._seconds;
				else
					return _minutes > op2._minutes;
			}
			return _hours > op2._hours;
		}



		bool Hour::operator > (const Hour& op2 ) const
		{
			if (_hours == op2._hours)
			{
				if (_minutes == op2._minutes)
					return _seconds > op2._seconds;
				else
					return _minutes > op2._minutes;
			}
			return _hours > op2._hours;
		}



		std::ostream& operator<< ( std::ostream& os, const Hour& op )
		{
			os << op.toSQLString(false);
			return os;
		}



		int Hour::operator-(const Hour& op2 ) const
		{
			int result = 0;

			// 1: Minutes
			int retain = ( MINUTES_PER_HOUR - 1 + op2._minutes -_minutes) / MINUTES_PER_HOUR;
			result += _minutes + retain * MINUTES_PER_HOUR - op2._minutes;

			// 2: Hours
			result += MINUTES_PER_HOUR * (_hours - op2._hours - retain );

			return result;
		}





		int
		Hour::operator+=( int minutesToAdd )
		{
			if (minutesToAdd < 0)
				return operator-=(-minutesToAdd);

			// 1: Minutes
			int calculatedTime = _minutes + minutesToAdd;
			int retain = calculatedTime / MINUTES_PER_HOUR;
			_minutes = calculatedTime % MINUTES_PER_HOUR;

			// 2: Hours
			calculatedTime = _hours + retain;
			retain = calculatedTime / HOURS_PER_DAY;
			_hours = calculatedTime % HOURS_PER_DAY;

			return retain;

		}




		int
		Hour::operator-=( int minutesToSubstract )
		{
			if (minutesToSubstract < 0)
				return operator+=(-minutesToSubstract);

			// 1: Minutes
			int retain = ( MINUTES_PER_HOUR - 1 + minutesToSubstract - _minutes) / MINUTES_PER_HOUR;
			_minutes = retain * MINUTES_PER_HOUR + _minutes - minutesToSubstract;
			int hoursToSubstract = retain;

			// 2: Hours
			retain = ( HOURS_PER_DAY - 1 + hoursToSubstract - _hours) / HOURS_PER_DAY;
			_hours = retain * HOURS_PER_DAY + _hours - hoursToSubstract;
			return retain;
		}



		Hour& Hour::operator = ( const Schedule& op )
		{
			return operator=(op.getHour());
		}



		Hour Hour::FromSQLTime (const std::string& sqlTime)
		{
			return FromString(sqlTime);
		}



		Hour Hour::FromString (const std::string& str)
		{
			typedef tokenizer<char_separator<char> > tokenizer;
			char_separator<char> sep (":","",keep_empty_tokens);
			tokenizer columns (str, sep);


			tokenizer::iterator it = columns.begin();
			int hour = Conversion::ToInt(*it);
			++it;
			if (it == columns.end())
				throw TimeParseException("Invalid hour");

			int minutes = Conversion::ToInt(*it);
			++it;
			int seconds(0);
			if (it != columns.end())
				seconds = Conversion::ToInt(*it);

			return Hour(hour, minutes, seconds);
		}

		std::string Hour::toSQLString( bool withApostrophes /*= true*/ ) const
		{
				
			if (isUnknown())
				return "";

			std::stringstream os;

			if (withApostrophes)
				os << "'"; 

			os << std::setw( 2 ) << std::setfill ( '0' )
				<< _hours << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< _minutes << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< _seconds
				;

			if (withApostrophes)
				os << "'"; 

			return os.str ();
		}



		int Hour::getSecondsDifference( const Hour& op2 ) const
		{
			int result = 0;

			// 0 : Seconds
			int retain = ( 59 + op2._seconds -_seconds) / 60;
			result = _seconds + retain * 60 - op2._seconds;

			// 1: Minutes
			int retain2 = ( 3599 + op2._minutes -_minutes) / 3600;
			result += 60 * (_minutes + retain2 * 3600 - op2._minutes - retain);

			// 2: Hours
			result += 3600 * (_hours - op2._hours - retain2 );

			return result;
		}


	}
}

