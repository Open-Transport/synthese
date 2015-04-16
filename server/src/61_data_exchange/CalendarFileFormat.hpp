
/** CalendarFileFormat class header.
	@file CalendarFileFormat.hpp

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

#ifndef SYNTHESE_calendar_CalendarFileFormat_hpp__
#define SYNTHESE_calendar_CalendarFileFormat_hpp__

#include "Importer.hpp"

#include "ImportableTableSync.hpp"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplateTableSync;
		class CalendarTemplate;
	}

	namespace data_exchange
	{

		//////////////////////////////////////////////////////////////////////////
		/// Calendar import helpers library.
		///	@ingroup m31
		class CalendarFileFormat:
			public virtual impex::Importer
		{
		public:
			CalendarFileFormat(
				util::Env& env,
				const impex::Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			);



			calendar::CalendarTemplate* _getCalendarTemplate(
				impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync>& calendars,
				const std::string& code
			) const;
		};
}	}

#endif // SYNTHESE_calendar_CalendarFileFormat_hpp__

