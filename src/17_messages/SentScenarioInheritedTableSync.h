
/** SentScenarioInheritedTableSync class header.
	@file SentScenarioInheritedTableSync.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_SentScenarioInheritedTableSync_H__
#define SYNTHESE_SentScenarioInheritedTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "SQLiteInheritedRegistryTableSync.h"
#include "ScenarioTableSync.h"
#include "SentScenario.h"

namespace synthese
{
	namespace messages
	{
		/** SentScenario table synchronizer.
			@ingroup m17LS refLS
		*/
		class SentScenarioInheritedTableSync
			: public db::SQLiteInheritedRegistryTableSync<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>
		{
		public:
			
			SentScenarioInheritedTableSync();



			typedef enum
			{
				BROADCAST_OVER,
				BROADCAST_RUNNING,
				BROADCAST_RUNNING_WITH_END,
				BROADCAST_RUNNING_WITHOUT_END,
				FUTURE_BROADCAST,
				BROADCAST_DRAFT
			} StatusSearch;

			

			/** Sent scenario search.
				@param name Name of the template scenario
				@param status status of the message :
					- BROADCAST_OVER : endDate in past
					- BROADCAST_RUNNING : startDate in past or unknown + activated,
					- BROADCAST_RUNNING_WITH_END : startDate in past or unknown + endTime in future + activated
					- BROADCAST_RUNNING_WITHOUT_END : startDate in past or unknown + endTime unknown + activated,
					- FUTURE_BROADCAST : startDate in future + activated,
					- BROADCAST_DRAFT : endDate in future or unknown + not activated 
				@param first First Scenario object to answer
				@param number Number of Scenario objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				boost::optional<std::string> name = boost::optional<std::string>(),
				//AlarmConflict conflict,
				//AlarmLevel level,
				boost::optional<StatusSearch> status = boost::optional<StatusSearch>(),
				boost::optional<time::DateTime> date = boost::optional<time::DateTime>(),
				boost::optional<util::RegistryKeyType> scenarioId = boost::optional<util::RegistryKeyType>(),
				boost::optional<int> first = boost::optional<int>(),
				boost::optional<int> number = boost::optional<int>(),
				bool orderByDate = true,
				bool orderByName = false,
				bool orderByStatus = false,
				//bool orderByConflict = false,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);

			
			static void CopyMessagesFromTemplate(
				util::RegistryKeyType sourceId,
				const SentScenario& dest
			);
		
		
			static void CopyMessagesFromOther(
				util::RegistryKeyType sourceId,
				const SentScenario& dest
			);
		};
	}
}

#endif // SYNTHESE_SentScenarioInheritedTableSync_H__
