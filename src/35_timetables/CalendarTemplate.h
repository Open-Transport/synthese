
/** CalendarTemplate class header.
	@file CalendarTemplate.h

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

#ifndef SYNTHESE_timetables_CalendarTemplate_h__
#define SYNTHESE_timetables_CalendarTemplate_h__

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "35_timetables/CalendarTemplateElement.h"

#include "15_env/Calendar.h"

namespace synthese
{
	namespace timetables
	{
		/** CalendarTemplate class.
			@ingroup m55
		*/
		class CalendarTemplate : public util::Registrable<uid,CalendarTemplate>
		{
			std::vector<CalendarTemplateElement>	_elements;
			std::string								_text;

		public:
			CalendarTemplate();

			//! @name Queries
			//@{
				env::Calendar	getCalendar(const env::Calendar& mask)	const;
			//@}

			//! @name Getters
			//@{
				std::string	getText()	const;
			//@}

			//! @name Setters
			//@{ 
				void	setText(const std::string& text);
			//@}

			//! @name Modifiers
			//@{
				void	addElement(const CalendarTemplateElement& element);
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_CalendarTemplate_h__
