
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

#include "17_messages/Types.h"

#include "01_util/UId.h"

namespace synthese
{
	namespace messages
	{
		/** Alarm message.
			@ingroup m17

			An alarm message is intended to be broadcasted at a time period into several destinations :
				- display screens
				- route planner results
				- etc.

			An alarm can be sent individually (single alarm) or in a group builded from a scenario (grouped alarm)
			The _scenario attribute points to the group if applicable.
		*/
		class Alarm
		{
		private:
			AlarmLevel					_level;
			std::string					_shortMessage;  //!< Alarm message
			std::string					_longMessage;  //!< Alarm message
		    
		protected:
			Alarm();
			Alarm(const Alarm& source);
			
		public:
			~Alarm();

			//! @name Getters/Setters
			//@{
				const std::string&		getShortMessage()	const;
				const std::string&		getLongMessage()	const;
				const AlarmLevel&		getLevel()			const;
				virtual uid				getId()				const = 0;

				void setLevel (const AlarmLevel& level);
				void setShortMessage( const std::string& message);
				void setLongMessage( const std::string& message);
			//@}
		};
	}
}

#endif
