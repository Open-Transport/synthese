
/** Hour class header.
	@file Hour.h

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

#ifndef SYNTHESE_TIME_HOUR_H
#define SYNTHESE_TIME_HOUR_H

#include "01_util/Constants.h"

#include "module.h"

#include <iostream>
#include <string>

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace time
	{

		class Schedule;

		/** Hour.
			@ingroup m04
		*/
		class Hour
		{
				int _hours;		//!< Hours
				int _minutes;	//!< Minutes
				int _seconds;	//!< Seconds

			public :

				/** Sets time including special characters conventions.
					@param hour Hours (A = current, M=maximum, m=minimum, _=unchanged)
					@param minutes Minutes (A=current, M=maximum, m=minimum,
						_=unchanged, I=same than hours)
				*/
				Hour ( int hours, int minutes = TIME_SAME, int seconds= TIME_SAME);
				Hour ( const Hour& ref );


				~Hour();

				//! @name Getters/Setters
				//@{
				int getMinutes() const { return _minutes; }
				int getHours() const { return _hours; }
				int getSeconds() const { return _seconds; }
				//@}


				//! @name Query methods
				//@{
					bool isValid () const;
					bool isUnknown () const;
					
					
					/** Human friendly hour display.
						@param withSeconds displays the seconds if true (default = false)
						@return std::string
						@author Hugues Romain
						@date 2008
						The output format is HH:MM:SS
					*/
					std::string toString (bool withSeconds = false) const;
					std::string toSQLString(bool withApostrophes = true) const;
				//@}



				/** Add minutes to this hour and return the number of
				day to add if needed (retain).
				@warning Only positive durations are handled!
				*/
				int operator += ( int );

				/** Substract minutes to this hour and return the number of
				day to substract if needed (retain)
				@warning Only positive durations are handled!
				*/
				int operator -= ( int );


				Hour& operator = ( const Schedule& op );

			static Hour FromString (const std::string& str);
			static Hour FromSQLTime (const std::string& sqlTime);


			bool operator == (const Hour& op2 ) const;
			bool operator != (const Hour& op2 ) const;
			bool operator <= (const Hour& op2 ) const;
			bool operator <  (const Hour& op2 ) const;
			bool operator >= (const Hour& op2 ) const;
			bool operator >  (const Hour& op2 ) const;

			/** Number of minutes elapsed between two hours.
			*/
			int operator- (const Hour& op2 ) const;

			boost::posix_time::time_duration toPosixTimeDuration() const;
			static Hour FromTimeDuration(boost::posix_time::time_duration value);
		};

		std::ostream& operator<< ( std::ostream& os, const Hour& op );
	}
}

#endif
