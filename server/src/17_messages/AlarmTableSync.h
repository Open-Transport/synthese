/** AlarmTableSync class header.
	@file AlarmTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H

#include "DBDirectTableSyncTemplate.hpp"
#include "FullSynchronizationPolicy.hpp"
#include "InheritanceLoadSavePolicy.hpp"

#include "Alarm.h"
#include "MessagesTypes.h"

#include "Exception.h"

#include <string>

namespace synthese
{
	namespace messages
	{
		class SentAlarm;
		class AlarmTemplate;
		class SentScenario;
		class ScenarioTemplate;
		class Scenario;


		/** Alarm table synchronizer.
			@ingroup m17LS refLS

			Only the sent alarms are loaded in ram.

			@note As Alarm is an abstract class, do not use the get static method. Use getAlarm instead.
		*/
		class AlarmTableSync:
			public db::DBDirectTableSyncTemplate<
				AlarmTableSync,
				Alarm,
				db::FullSynchronizationPolicy,
				db::InheritanceLoadSavePolicy
			>
		{
		protected:
			static const std::string _COL_CONFLICT_LEVEL;
			static const std::string _COL_RECIPIENTS_NUMBER;

		public:
			static const std::string COL_LEVEL;
			static const std::string COL_IS_TEMPLATE;
			static const std::string COL_SHORT_MESSAGE;
			static const std::string COL_LONG_MESSAGE;
			static const std::string COL_SCENARIO_ID;
			static const std::string COL_TEMPLATE_ID;
			static const std::string COL_RAW_EDITOR;
			static const std::string COL_DONE;
			static const std::string COL_MESSAGES_SECTION_ID;
			static const std::string COL_CALENDAR_ID;
			static const std::string COL_DATASOURCE_LINKS;
			static const std::string COL_DISPLAY_DURATION;
			static const std::string COL_DIGITIZED_VERSION;
			static const std::string COL_TAGS;

			AlarmTableSync() {}
			~AlarmTableSync() {}

			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> scenarioId = boost::optional<util::RegistryKeyType>()
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByLevel = false
				, bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif
