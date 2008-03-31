
/** ScenarioInheritedTableSync class header.
	@file ScenarioInheritedTableSync.h

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

#ifndef SYNTHESE_messages_ScenarioInheritedTableSync_h__
#define SYNTHESE_messages_ScenarioInheritedTableSync_h__

#include "17_messages/AlarmTableSync.h"

#include "02_db/DBModule.h"

namespace synthese
{
	namespace messages
	{
		/** @addtogroup m17
		@{
		*/

		template <class S>
		static void SaveAlarms( ScenarioSubclassTemplate<S>* object )
		{
			// save(object);
			typename ScenarioSubclassTemplate<S>::AlarmsSet& alarms = object->getAlarms();
			for (typename ScenarioSubclassTemplate<S>::AlarmsSet::iterator it = alarms.begin(); it != alarms.end(); ++it)
			{
				AlarmTableSync::Save(static_cast<Alarm*>(*it));

				/// @todo Saving of the broadcast list
			}

		}


		template<class ScenarioSubClass>
		static void LoadScenarioAlarms(ScenarioSubClass* scenario)
		{
			db::SQLite* sqlite = db::DBModule::GetSQLite();
			std::stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << AlarmTableSync::TABLE_NAME << " AS a "
				<< " WHERE " << AlarmTableSync::COL_SCENARIO_ID << "=" << scenario->getKey();
			try
			{
				db::SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				while (rows->next ())
				{
					typename ScenarioSubClass::AlarmType* object(new typename ScenarioSubClass::AlarmType(scenario));
					scenario->addChildTemporaryObject(object);
					AlarmTableSync::Load(object, rows);
					scenario->addAlarm(object);
				}
			}
			catch(db::SQLiteException& e)
			{
				throw util::Exception(e.getMessage());
			}

		}

		/** @} */
	}
}

#endif // SYNTHESE_messages_ScenarioInheritedTableSync_h__
