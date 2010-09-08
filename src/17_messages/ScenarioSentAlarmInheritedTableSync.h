
/** ScenarioSentAlarmInheritedTableSync class header.
	@file ScenarioSentAlarmInheritedTableSync.h

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

#ifndef SYNTHESE_messages_ScenarioSentAlarmInheritedTableSync_h__
#define SYNTHESE_messages_ScenarioSentAlarmInheritedTableSync_h__

#include "SQLiteInheritedRegistryTableSync.h"

#include "AlarmTableSync.h"
#include "SentAlarm.h"

namespace synthese
{
	namespace messages
	{
		/** ScenarioSentAlarmInheritedTableSync class.
			@ingroup m17ILS refILS
		*/
		class ScenarioSentAlarmInheritedTableSync
		:	public db::SQLiteInheritedRegistryTableSync<
				AlarmTableSync, ScenarioSentAlarmInheritedTableSync, SentAlarm
			>
		{
		public:
			static SearchResult Search(
				util::Env& env,
				util::RegistryKeyType scenarioId,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByLevel = false
				, bool orderByStatus = false
				, bool orderByConflict = false
				, bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_messages_ScenarioSentAlarmInheritedTableSync_h__
