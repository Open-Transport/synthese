
/** CalendarFileFormat class implementation.
	@file CalendarFileFormat.cpp

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

#include "CalendarFileFormat.hpp"

#include "CalendarTemplateTableSync.h"

using namespace std;

namespace synthese
{
	using namespace calendar;
	using namespace impex;
	
	namespace data_exchange
	{
		CalendarFileFormat::CalendarFileFormat(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		CalendarTemplate* CalendarFileFormat::_getCalendarTemplate(
			impex::ImportableTableSync::ObjectBySource<CalendarTemplateTableSync>& calendars,
			const std::string& code
		) const {
			// Search for a road place linked with the datasource
			if(calendars.contains(code))
			{
				set<CalendarTemplate*> loadedCalendars(calendars.get(code));
				if(loadedCalendars.size() > 1)
				{
					_logWarning(
						"More than one calendar template with key "+ code
					);
				}
				return *loadedCalendars.begin();
			}
			else
			{
				_logWarning(
					"No calendar template with key "+ code
				);
				return NULL;
			}
		}
}	}

