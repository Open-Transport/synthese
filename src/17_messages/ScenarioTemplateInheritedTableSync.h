
/** ScenarioTemplateInheritedTableSync class header.
	@file ScenarioTemplateInheritedTableSync.h

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

#ifndef SYNTHESE_messages_ScenarioTemplateInheritedTableSync_h__
#define SYNTHESE_messages_ScenarioTemplateInheritedTableSync_h__

#include "SQLiteInheritedNoSyncTableSyncTemplate.h"
#include "ScenarioTableSync.h"
#include "ScenarioTemplate.h"

namespace synthese
{
	namespace messages
	{
		/** ScenarioTemplateInheritedTableSync class.
			@ingroup m17
		*/
		class ScenarioTemplateInheritedTableSync
		:	public db::SQLiteInheritedNoSyncTableSyncTemplate<
				ScenarioTableSync, ScenarioTemplateInheritedTableSync, ScenarioTemplate
			>
		{
		public:
			ScenarioTemplateInheritedTableSync();

			/** Template scenario search.
				@param env Environment to populate
				@param name Name of the template
				@param first First Scenario object to answer
				@param number Number of Scenario objects to answer (0 = all)
				The size of the vector is less or equal to number, then all users were returned despite of 
				the number limit. If the size is greater than number (actually equal to number + 1) then 
				there is others accounts to show. Test it to know if the situation needs a "click for more"
				button.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				uid folderId
				, const std::string name = std::string()
				, const ScenarioTemplate* scenarioToBeDifferentWith = NULL
				, int first = 0
				, int number = -1
				, bool orderByName = true
				, bool raisingOrder = false,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
			
			
			
			static ScenarioTemplate::VariablesMap GetVariables(
				util::RegistryKeyType scenarioId
			);
			
			
			
			static void CopyMessagesFromOther(
				util::RegistryKeyType sourceId,
				const ScenarioTemplate& dest
			);
		};
	}
}

#endif // SYNTHESE_messages_ScenarioTemplateInheritedTableSync_h__
