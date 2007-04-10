
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



namespace synthese
{
	namespace time
	{

		class Schedule;

		/** Heure
		@ingroup m04
		*/
		class Hour
		{
				int _hours; //!< Hours
				int _minutes; //!< Minutes

			public :

				Hour ( int hours = UNKNOWN_VALUE, int minutes = UNKNOWN_VALUE );
				Hour ( const Hour& ref );


				~Hour();

				//! @name Getters/Setters
				//@{
				inline int getMinutes() const;
				inline int getHours() const;
				//@}


				//! @name Query methods
				//@{
					bool isValid () const;
					bool isUnknown () const;
					std::string toString () const;
					std::string toInternalString () const;
					std::string toSQLString(bool withApostrophes = true) const;
				//@}


				//! @name Update methods
				//@{

				/** Sets time including special characters conventions.
				@param hour Hours (A = current, M=maximum, m=minimum, _=unchanged)
				@param minutes Minutes (A=current, M=maximum, m=minimum,
										_=unchanged, I=same than hours)
				*/
				void updateHour ( int hour = TIME_CURRENT,
								int minutes = TIME_SAME );

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

				/** @param op Updates this hour according to the meaning of
							the given 4 special characters ( m => minimum, M =>
							maximum...  max = 5 chars).
				*/
				Hour& operator = ( const std::string& op );

				Hour& operator = ( const Schedule& op );

			static Hour FromString (const std::string& str);
			static Hour FromSQLTime (const std::string& sqlTime);


		};


		bool operator == ( const Hour& op1, const Hour& op2 );
		bool operator != ( const Hour& op1, const Hour& op2 );
		bool operator <= ( const Hour& op1, const Hour& op2 );
		bool operator < ( const Hour& op1, const Hour& op2 );
		bool operator >= ( const Hour& op1, const Hour& op2 );
		bool operator > ( const Hour& op1, const Hour& op2 );

		/** Number of minutes elapsed between two hours.
		*/
		int operator- ( const Hour& op1, const Hour& op2 );

		std::ostream& operator<< ( std::ostream& os, const Hour& op );

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


	}
}

#endif
