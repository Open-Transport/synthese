
/** NonConcurrencyRuleTableSync class header.
	@file NonConcurrencyRuleTableSync.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_NonConcurrencyRuleTableSync_H__
#define SYNTHESE_NonConcurrencyRuleTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"
#include "NonConcurrencyRule.h"

namespace synthese
{
	namespace pt
	{
		/** NonConcurrencyRule table synchronizer.
			@ingroup m35LS refLS
		*/
		class NonConcurrencyRuleTableSync:
			public db::DBDirectTableSyncTemplate<
				NonConcurrencyRuleTableSync,
				NonConcurrencyRule
			>
		{
		public:
			/** NonConcurrencyRule search.
				@param hiddenLineId ID of the hidden line to filter (undefined = no filter)
				@param priorityLineId ID of the priority line to filter (undefined = no filter)
				@param hiddenAndPriority true indicates that the two filters must be passed, false indicates that at least one of them must be passed
				@param first First NonConcurrencyRule object to answer
				@param number Number of NonConcurrencyRule objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found NonConcurrencyRule objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> hiddenLineId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> priorityLineId = boost::optional<util::RegistryKeyType>(),
				bool hiddenAndPriority = true
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByPriorityLine = true
				, bool orderByHiddenLine = false
				, bool orderByDelay = false
				, bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_NonConcurrencyRuleTableSync_H__
