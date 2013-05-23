
/** ScenarioCalendarTableSync class header.
	@file ScenarioCalendarTableSync.hpp

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

#ifndef SYNTHESE_ScenarioCalendarTableSync_H__
#define SYNTHESE_ScenarioCalendarTableSync_H__

#include "ScenarioCalendar.hpp"

#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace messages
	{
		//////////////////////////////////////////////////////////////////////////
		/// 17.10 Table : Message alternative.
		///	@ingroup m17LS refLS
		/// @author Hugues Romain
		/// @date 2012
		/// @since 3.5.0
		//////////////////////////////////////////////////////////////////////////
		class ScenarioCalendarTableSync:
			public db::DBDirectTableSyncTemplate<ScenarioCalendarTableSync, ScenarioCalendar>
		{
		public:
			/** Message type search.
				@param first First Message type to answer
				@param number Number of Message types to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found message types.
				@author Hugues Romain
				@date 2012
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> scenarioId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<size_t> number = boost::optional<size_t>(),
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif
