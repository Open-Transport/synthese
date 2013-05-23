
/** Messages module related types definitions file.
	@file MessagesTypes.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_messages_Types_H__
#define SYNTHESE_messages_Types_H__

#include <string>
#include <map>

namespace synthese
{
	namespace messages
	{
		/** @addtogroup m17
			@{
		*/

		/** Alarm level.

		- INFO : the solution is available, but some information is
		provided for better comfort

		- WARNING : the solution could not be available, or requires action
		from the passenger (reservation...)
		*/
		typedef enum
		{
			ALARM_LEVEL_NO_ALARM = 0,
			ALARM_LEVEL_UNKNOWN = 1,
			ALARM_LEVEL_INFO = 10,
			ALARM_LEVEL_ALTERNATE = 30,
			ALARM_LEVEL_WARNING = 50,
			ALARM_LEVEL_SCENARIO = 100
		} AlarmLevel;



		typedef struct
		{
			std::string label;
			std::string htmlField;
			std::string query;
		} AlarmRecipientFilter;

		typedef std::map<std::string, AlarmRecipientFilter> AlarmRecipientSearchFieldsMap;

		/** @} */
	}
}

#endif // SYNTHESE_messages_Types_H__
