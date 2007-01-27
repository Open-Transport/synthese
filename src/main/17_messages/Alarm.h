
/** Alarm class header.
	@file Alarm.h

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

#ifndef SYNTHESE_MESSAGES_ALARM_H
#define SYNTHESE_MESSAGES_ALARM_H

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"


namespace synthese
{
	namespace messages
	{
		/** Alarm message.
			@ingroup m17
		*/
		class Alarm : public synthese::util::Registrable<uid,Alarm>
		{
		public:

			/** Alarm level.

			- INFO : the solution is available, but some information is 
			provided for better comfort

			- WARNING : the solution could not be available, or requires action 
			from the passenger (reservation...)
			*/
			typedef enum { ALARM_LEVEL_INFO, ALARM_LEVEL_WARNING, ALARM_LEVEL_ERROR } AlarmLevel; 

		private:

			std::string _message;  //!< Alarm message
			synthese::time::DateTime _periodStart; //!< Alarm applicability period start
			synthese::time::DateTime _periodEnd;   //!< Alarm applicability period end
			AlarmLevel _level; 
		    
		public:

			Alarm(const uid& id);
			Alarm (const uid& id,
			const std::string& message, 
			const synthese::time::DateTime& periodStart,
			const synthese::time::DateTime& periodEnd,
			const AlarmLevel& alarmLevel
			);
		    
			//! @name Getters/Setters
			//@{

			const std::string& getMessage () const;
			void setMessage( const std::string& message);

			const AlarmLevel& getLevel () const;
			void setLevel (const AlarmLevel& level);

			void setPeriodStart ( const synthese::time::DateTime& periodStart);
			void setPeriodEnd ( const synthese::time::DateTime& periodEnd);

			//@}
		    
		    
			//! \name Calculateurs
			//@{

			/** Applicability test.
			@param start Start of applicability period
			@param end End of applicability period
			@return true if the message is not empty and 
			is valid for the whole period given as argument.
			*/
			bool isApplicable ( const synthese::time::DateTime& start, 
					const synthese::time::DateTime& end ) const;
			//@}
		    
		};
	}
}

#endif
