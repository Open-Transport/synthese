
/** CalendarModule class header.
	@file CalendarModule.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_CalendarModule_H__
#define SYNTHESE_CalendarModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m19Actions 19 Actions
		@ingroup m19

		@defgroup m19Pages 19 Pages
		@ingroup m19

		@defgroup m19Functions 19 Functions
		@ingroup m19

		@defgroup m19Exceptions 19 Exceptions
		@ingroup m19

		@defgroup m19Alarm 19 Messages recipient
		@ingroup m19

		@defgroup m19LS 19 Table synchronizers
		@ingroup m19

		@defgroup m19Admin 19 Administration pages
		@ingroup m19

		@defgroup m19Rights 19 Rights
		@ingroup m19

		@defgroup m19Logs 19 DB Logs
		@ingroup m19
		
		@defgroup m19 calendar
		@ingroup m1
		
		(Module documentation)
		
		@{
	*/

	/** 19 calendar Module namespace.
		@author Hugues
		@date 2009
	*/
	namespace calendar
	{
		class Calendar;

		/** 19 calendar Module class.
			@author Hugues
			@date 2009
		*/
		class CalendarModule:
			public server::ModuleClassTemplate<CalendarModule>
		{
		public:
			static std::string GetBestCalendarTitle(
				const Calendar& calendar,
				const Calendar& mask
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_CalendarModule_H__
