
/** CalendarLinkTableSync class header.
	@file CalendarLinkTableSync.hpp

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

#ifndef SYNTHESE_ServiceCalendarLinkTableSync_H__
#define SYNTHESE_ServiceCalendarLinkTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "CalendarLink.hpp"
#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

namespace synthese
{
	namespace calendar
	{
		class CalendarLink;

		/** ServiceCalendarLink table synchronizer.
			@ingroup m31LS refLS
		*/
		class CalendarLinkTableSync:
			public db::DBDirectTableSyncTemplate<
				CalendarLinkTableSync,
				CalendarLink,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			static const std::string COL_SERVICE_ID;
			static const std::string COL_START_DATE;
			static const std::string COL_END_DATE;
			static const std::string COL_CALENDAR_TEMPLATE_ID;
			static const std::string COL_CALENDAR_TEMPLATE_ID2;


			/** ServiceCalendarLink search.
				@param commercialLineId Id of commercial line which must belong the results (undefined = all lines)
				@param first First ServiceCalendarLink object to answer
				@param number Number of ServiceCalendarLink objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found ServiceCalendarLink objects.
				@author Hugues Romain
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> serviceId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByService = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif
