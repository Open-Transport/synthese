
/** CalendarModule class header.
	@file CalendarModule.h
	@author Hugues Romain
	@date 2009

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

#ifndef SYNTHESE_CalendarModule_H__
#define SYNTHESE_CalendarModule_H__

#include "ModuleClassTemplate.hpp"
#include "Calendar.h"

namespace synthese
{
	/**	@defgroup m31Actions 31 Actions
		@ingroup m31

		@defgroup m31Pages 31 Pages
		@ingroup m31

		@defgroup m31Functions 31 Functions
		@ingroup m31

		@defgroup m31Exceptions 31 Exceptions
		@ingroup m31

		@defgroup m31Alarm 31 Messages recipient
		@ingroup m31

		@defgroup m31LS 31 Table synchronizers
		@ingroup m31

		@defgroup m31Admin 31 Administration pages
		@ingroup m31

		@defgroup m31Rights 31 Rights
		@ingroup m31

		@defgroup m31Logs 31 DB Logs
		@ingroup m31

		@defgroup m31 31 Calendar
		@ingroup m3

		@{
	*/

	/** 31 calendar Module namespace.
		@author Hugues Romain
		@date 2009
	*/
	namespace calendar
	{
		class CalendarTemplate;

		/** 31 calendar Module class.
			@author Hugues Romain
			@date 2009
		*/
		class CalendarModule:
			public server::ModuleClassTemplate<CalendarModule>
		{
		public:
			typedef std::pair<CalendarTemplate*, std::string> BaseCalendar;

			class CalendarTitlesGenerator
			{
			private:
				typedef std::vector<std::pair<Calendar, CalendarTemplate*> > Value;

				Value _value;

			public:
				CalendarTitlesGenerator(const Calendar& mask);

				//////////////////////////////////////////////////////////////////////////
				/// @param calendar calendar to name (is supposed to be included by the mask)
				BaseCalendar getBestCalendarTitle(const Calendar& calendar);
			};


			static BaseCalendar GetBestCalendarTitle(
				const Calendar& calendar,
				const Calendar& mask
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_CalendarModule_H__
