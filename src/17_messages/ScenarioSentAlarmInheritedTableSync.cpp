
/** ScenarioSentAlarmInheritedTableSync class implementation.
	@file ScenarioSentAlarmInheritedTableSync.cpp

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

#include "ScenarioSentAlarmInheritedTableSync.h"
#include "SentScenarioInheritedTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace messages;

	template<>
	const string util::FactorableTemplate<AlarmTableSync, ScenarioSentAlarmInheritedTableSync>::FACTORY_KEY("ScenarioSentAlarmInheritedTableSync");

	namespace db
	{

		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,ScenarioSentAlarmInheritedTableSync,ScenarioSentAlarm>::_Link(ScenarioSentAlarm* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			obj->setScenario(
				SentScenarioInheritedTableSync::Get(
					rows->getLongLong(AlarmTableSync::COL_SCENARIO_ID)
					, obj
					, false
				)
			);
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,ScenarioSentAlarmInheritedTableSync,ScenarioSentAlarm>::_Unlink(ScenarioSentAlarm* obj)
		{
			obj->setScenario(NULL);
		}
	}

	namespace messages
	{

		ScenarioSentAlarmInheritedTableSync::ScenarioSentAlarmInheritedTableSync()
			: SQLiteInheritedRegistryTableSync<AlarmTableSync, ScenarioSentAlarmInheritedTableSync, ScenarioSentAlarm>()
		{

		}
	}
}