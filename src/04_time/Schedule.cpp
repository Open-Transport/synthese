
/** Schedule class implementation.
	@file Schedule.cpp

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

#include "Schedule.h"
#include "assert.h"

#include "Conversion.h"

#include <sstream>
#include <iomanip>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace time
	{


		  Schedule::Schedule ()
			  : _hour(TIME_UNKNOWN)
			  , _daysSinceDeparture(0)
		  {
		  }


		Schedule::Schedule ( const Hour& hour, int daysSinceDeparture )
				: _hour ( hour )
				, _daysSinceDeparture ( daysSinceDeparture )
		{
			assert ( ( _daysSinceDeparture >= 0 ) &&
					 ( _daysSinceDeparture <= 28 ) );
		}



		Schedule::Schedule ( const Schedule& ref )
				: _hour ( ref._hour )
				, _daysSinceDeparture ( ref._daysSinceDeparture )
		{
			assert ( ( _daysSinceDeparture >= 0 ) &&
					 ( _daysSinceDeparture <= 28 ) );

		}


		Schedule::~Schedule ()
		{}


		const Hour&
		Schedule::getHour() const
		{
			return _hour;
		}



		int
		Schedule::getDaysSinceDeparture () const
		{
			return _daysSinceDeparture;
		}


		void
		Schedule::setDaysSinceDeparture ( int daysSinceDeparture )
		{
			_daysSinceDeparture = daysSinceDeparture;
		}


		bool 
		Schedule::isValid () const
		{
		  return(_daysSinceDeparture >= 0 && 
			 _daysSinceDeparture <=28 && 
			 _hour.isValid ());
		}



		void
		Schedule::setMinimum()
		{
			_daysSinceDeparture = 0;
			_hour = Hour(TIME_MIN );
		}


		void
		Schedule::setMaximum()
		{
			_daysSinceDeparture = 255;
			_hour = Hour(TIME_MAX );
		}


		Schedule&
		Schedule::operator += ( int op )
		{
			_daysSinceDeparture = ( _daysSinceDeparture + ( _hour += op ) );
			return ( *this );
		}




		Schedule Schedule::operator + (int op2 ) const
		{
			Schedule result(*this);
			result += op2;
			return result;
		}



		bool Schedule::operator < (const Schedule& op2 ) const
		{
			return	_daysSinceDeparture < op2._daysSinceDeparture
				||	_daysSinceDeparture == op2._daysSinceDeparture
					&&	 _hour < op2._hour
			;
		}



		bool Schedule::operator < (const Hour& op2 ) const
		{
			return _hour < op2;
		}



		bool Schedule::operator <= (const Schedule& op2 ) const
		{
			return	_daysSinceDeparture < op2._daysSinceDeparture
				||	_daysSinceDeparture == op2._daysSinceDeparture
					&&	_hour <= op2._hour
			;
		}



		bool Schedule::operator <= (const Hour& op2 ) const
		{
			return _hour <= op2;
		}



		bool Schedule::operator >= (const Schedule& op2 ) const
		{
			return	_daysSinceDeparture > op2._daysSinceDeparture
				||	_daysSinceDeparture == op2._daysSinceDeparture
					&&	_hour >= op2._hour
			;
		}



		bool Schedule::operator >= (const Hour& op2 ) const
		{
			return _hour >= op2;
		}




		bool Schedule::operator > (const Hour& op2 ) const
		{
			return _hour > op2;
		}



		int Schedule::operator - (const Schedule& op2 ) const
		{
			int retain = 0;

			// 1: Hour
			int result = _hour - op2._hour;

			if ( result < 0 )
			{
				retain = 1;
				result += MINUTES_PER_DAY;
			}

			// 2: Days since departure
			result += (_daysSinceDeparture - op2._daysSinceDeparture - retain) * MINUTES_PER_DAY;

			return result;
		}



		std::string Schedule::toSQLString( bool withApostrophes /*= true*/ ) const
		{
			if(!isValid())
				return "";

			std::stringstream os;

			if (withApostrophes)
				os << "'"; 

			os << std::setw( 2 ) << std::setfill ( '0' )
				<< _daysSinceDeparture << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< _hour.getHours() << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< _hour.getMinutes()
				;

			if (withApostrophes)
				os << "'"; 

			return os.str ();
		}




		std::ostream&
		operator << ( std::ostream& os, const Schedule& op )
		{
			os << op.getDaysSinceDeparture ();
			os << op.getHour ();
			return os ;
		}



		Schedule
		Schedule::FromString (const std::string& str)
		{
			// DD:hh:mm
			return Schedule (
			Hour (Conversion::ToInt (str.substr (3, 2)),
				  Conversion::ToInt (str.substr (6, 2))),
			Conversion::ToInt (str.substr (0, 2))
			);
		}

		std::string Schedule::toString() const
		{
			if (_hour.isUnknown())
				return string();

			std::stringstream os;
			if (_daysSinceDeparture)
				os << ((_daysSinceDeparture > 0) ? "+" : "") << _daysSinceDeparture << "/";

			os << _hour.toString();
			return os.str ();
		}

		bool Schedule::operator==(const Schedule& op2 ) const
		{
			return	_daysSinceDeparture == op2._daysSinceDeparture
				&&	_hour == op2._hour;
		}
	}
}

