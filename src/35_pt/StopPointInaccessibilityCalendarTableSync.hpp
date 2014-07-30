/** StopPointAccessibilityCalendarTableSync class header.
	@file StopPointAccessibilityCalendarTableSync.hpp

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

#ifndef SYNTHESE_ENVLSSQL_StopPointAccessibilityCalendarTableSync_H__
#define SYNTHESE_ENVLSSQL_StopPointAccessibilityCalendarTableSync_H__

#include "StopPointInaccessibilityCalendar.hpp"

#include "DBDirectTableSyncTemplate.hpp"
#include "GraphTypes.h"
#include "StandardLoadSavePolicy.hpp"

namespace synthese
{
	namespace pt
	{
		class StopPointInaccessibilityCalendarTableSync:
			public db::DBDirectTableSyncTemplate<
				StopPointInaccessibilityCalendarTableSync,
				StopPointInaccessibilityCalendar
			>
		{
		public:

			StopPointInaccessibilityCalendarTableSync() {}

			/** StopPointAccessiblity search.
				@param env Environment to read and populate
				@param stopPointId id of the associated stop point
				@param calendarId id of the associated calendar
				@param userClassCode the userClassCode for which the accessibility calendar exists
				@param first First StopPointAccessiblity object to answer
				@param number Number of StopPointAccessiblity objects to answer (0 = all)
				@param raisingOrder direction of ordering (true = ascendent order, false = descendent order)
				@param linkLevel level of recursion of object creation and link
				@author Erwan Turck
				@date 2014
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> stopPointId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> calendarId = boost::optional<util::RegistryKeyType>(),
				graph::UserClassCode userClassCode = graph::USER_CLASS_CODE_OFFSET,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_ENVLSSQL_StopPointAccessibilityCalendarTableSync_H__
