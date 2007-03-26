
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

#include "assert.h"

#include <iomanip>
#include <sstream>
#include <ctime>

#include "01_util/Conversion.h"

#include "04_time/Hour.h"
#include "04_time/Schedule.h"
#include "04_time/TimeParseException.h"

using synthese::util::Conversion;


namespace synthese
{
namespace time
{


Hour::Hour ( int hours, int minutes )
        : _hours ( hours )
        , _minutes ( minutes )
{}


Hour::Hour ( const Hour& ref )
        : _hours ( ref._hours )
        , _minutes ( ref._minutes )
{
}


Hour::~Hour()
{}



int
Hour::getMinutes() const
{
    return _minutes;
}


int
Hour::getHours() const
{
    return _hours;
}



bool
Hour::isValid () const
{
    return ( _hours >= 0 ) && ( _hours < HOURS_PER_DAY ) &&
           ( _minutes >= 0 ) && ( _minutes < MINUTES_PER_HOUR );
}



bool
Hour::isUnknown () const
{
    return ( _hours == UNKNOWN_VALUE ) || ( _minutes == UNKNOWN_VALUE );
}


std::string Hour::toString () const
{
	if (isUnknown())
		return "";

	std::stringstream os;
	os << std::setw( 2 ) << std::setfill ( '0' )
		<< getHours () << ":" 
		<< std::setw( 2 ) << std::setfill ( '0' )
		<< getMinutes ();
	return os.str ();
}


std::string 
Hour::toInternalString () const
{
	if (isUnknown())
		return "";

	std::stringstream os;

	os << std::setw( 2 ) << std::setfill ( '0' )
		<< getHours ()
		<< std::setw( 2 ) << std::setfill ( '0' )
		<< getMinutes ();

	return os.str ();
}


bool
operator == ( const Hour& op1, const Hour& op2 )
{
    return ( op1.getHours () == op2.getHours () ) &&
           ( op1.getMinutes () == op2.getMinutes () );
}



bool
operator != ( const Hour& op1, const Hour& op2 )
{
    return ( op1.getHours () != op2.getHours () ) ||
           ( op1.getMinutes () != op2.getMinutes () );
}



bool
operator <= ( const Hour& op1, const Hour& op2 )
{
    if ( op1.getHours () == op2.getHours () )
        return op1.getMinutes () <= op2.getMinutes ();

    return op1.getHours () < op2.getHours ();
}



bool
operator<( const Hour& op1, const Hour& op2 )
{
    if ( op1.getHours () == op2.getHours () )
        return op1.getMinutes () < op2.getMinutes ();

    return op1.getHours () < op2.getHours ();
}


bool
operator>=( const Hour& op1, const Hour& op2 )
{
    if ( op1.getHours () == op2.getHours () )
        return op1.getMinutes () >= op2.getMinutes ();

    return op1.getHours () > op2.getHours ();

}



bool
operator > ( const Hour& op1, const Hour& op2 )
{
    if ( op1.getHours () == op2.getHours () )
        return op1.getMinutes () > op2.getMinutes ();

    return op1.getHours () > op2.getHours ();

}



std::ostream& operator<< ( std::ostream& os, const Hour& op )
{
    os << std::setw( 2 ) << std::setfill ( '0' )
    << op.getHours ()
    << std::setw( 2 ) << std::setfill ( '0' )
    << op.getMinutes ();
    return os;
}



int
operator-( const Hour& op1, const Hour& op2 )
{
    int result = 0;

    // 1: Minutes
    int retain = ( MINUTES_PER_HOUR - 1 + op2.getMinutes() - op1.getMinutes() ) / MINUTES_PER_HOUR;
    result += op1.getMinutes () + retain * MINUTES_PER_HOUR - op2.getMinutes ();

    // 2: Hours
    result += MINUTES_PER_HOUR * ( op1.getHours () - op2.getHours() - retain );

    return result;
}





int
Hour::operator+=( int minutesToAdd )
{
    // 1: Minutes
    int calculatedTime = getMinutes () + minutesToAdd;
    int retain = calculatedTime / MINUTES_PER_HOUR;
    _minutes = calculatedTime % MINUTES_PER_HOUR;

    // 2: Hours
    calculatedTime = getHours () + retain;
    retain = calculatedTime / HOURS_PER_DAY;
    _hours = calculatedTime % HOURS_PER_DAY;

    return retain;

}




int
Hour::operator-=( int minutesToSubstract )
{
    // 1: Minutes
    int retain = ( MINUTES_PER_HOUR - 1 + minutesToSubstract - getMinutes() ) / MINUTES_PER_HOUR;
    _minutes = retain * MINUTES_PER_HOUR + _minutes - minutesToSubstract;
    int hoursToSubstract = retain;

    // 2: Hours
    retain = ( HOURS_PER_DAY - 1 + hoursToSubstract - getHours () ) / HOURS_PER_DAY;
    _hours = retain * HOURS_PER_DAY + getHours () - hoursToSubstract;
    return retain;
}




Hour&
Hour::operator = ( const Schedule& op )
{
    _hours = op.getHours();
    _minutes = op.getMinutes ();
    return ( *this );
}



Hour&
Hour::operator = ( const std::string& op )
{
    if ( op.size () == 0 )
    {
        _hours = 0;
        _minutes = 0;
    }
    else if ( op.size () == 1 )
    {
        updateHour ( op[ 0 ] );
    }
    else
    {
        _hours = atoi ( op.substr ( 0, 2 ).c_str () );
        _minutes = atoi ( op.substr ( 2, 2 ).c_str () );
    }
    return ( *this );
}



void
Hour::updateHour ( int hours, int minutes )
{
    time_t rawtime;
    struct tm * timeinfo = 0;

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
    else if ( minutes != TIME_UNCHANGED && ( minutes != TIME_SAME || hours != TIME_UNCHANGED ) )
        _minutes = minutes;
}



Hour
Hour::FromSQLTime (const std::string& sqlTime)
{
	int dot = (int) sqlTime.find(':');

	if (dot == -1)
		throw TimeParseException("Invalid hour");

	return Hour(Conversion::ToInt (sqlTime.substr (0, dot)),
		Conversion::ToInt (sqlTime.substr (dot+1, sqlTime.length() - dot))
	);
}


Hour
Hour::FromString (const std::string& str)
{
	int dot = (int) str.find(':');

	if (dot == -1)
		throw TimeParseException("Invalid hour");

	return Hour(Conversion::ToInt (str.substr (0, dot)),
		Conversion::ToInt (str.substr (dot+1, str.length() - dot))
	);
}

std::string Hour::toSQLString( bool withApostrophes /*= true*/ ) const
{
	return (withApostrophes ? "'" : "") + Conversion::ToString(_hours) + ":" + Conversion::ToString(_minutes) + ":00" + (withApostrophes ? "'" : "");
}



}
}

