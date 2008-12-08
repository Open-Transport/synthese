
/** ScenarioTableSync class implementation.
	@file ScenarioTableSync.cpp

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

#include "17_messages/ScenarioTableSync.h"
#include "17_messages/SentScenario.h"
#include "17_messages/SentScenarioInheritedTableSync.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTemplateInheritedTableSync.h"
#include "17_messages/AlarmTableSync.h"

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "04_time/DateTime.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,ScenarioTableSync>::FACTORY_KEY("17.00.01 Alarm scenarii");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ScenarioTableSync>::TABLE_NAME = "t039_scenarios";
		template<> const int SQLiteTableSyncTemplate<ScenarioTableSync>::TABLE_ID = 39;
		template<> const bool SQLiteTableSyncTemplate<ScenarioTableSync>::HAS_AUTO_INCREMENT = true;


		template<>
		string SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::_GetSubClassKey(const SQLiteResultSPtr& row)
		{
			return row->getBool(ScenarioTableSync::COL_IS_TEMPLATE)
				? ScenarioTemplateInheritedTableSync::FACTORY_KEY
				: SentScenarioInheritedTableSync::FACTORY_KEY
				;
		}



		template<>
		string SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::_GetSubClassKey(const Scenario* obj)
		{
			return (dynamic_cast<const SentScenario*>(obj) != NULL)
				?	SentScenarioInheritedTableSync::FACTORY_KEY
				:	ScenarioTemplateInheritedTableSync::FACTORY_KEY
				;
		}



		template<> void SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::_CommonLoad(
			Scenario* object,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( ScenarioTableSync::COL_NAME));
		}
	}

	namespace messages
	{
		const std::string ScenarioTableSync::COL_IS_TEMPLATE = "is_template";
		const std::string ScenarioTableSync::COL_ENABLED = "is_enabled";
		const std::string ScenarioTableSync::COL_NAME = "name";
		const std::string ScenarioTableSync::COL_PERIODSTART = "period_start";
		const std::string ScenarioTableSync::COL_PERIODEND = "period_end"; 
		const std::string ScenarioTableSync::COL_FOLDER_ID("folder_id");

		
		ScenarioTableSync::ScenarioTableSync()
			: SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_IS_TEMPLATE, "INTEGER");
			addTableColumn(COL_ENABLED, "INTEGER");
			addTableColumn(COL_NAME, "TEXT");
			addTableColumn (COL_PERIODSTART, "TIMESTAMP", true);
			addTableColumn (COL_PERIODEND, "TIMESTAMP", true);
			addTableColumn(COL_FOLDER_ID, "INTEGER");

			vector<string> cols;
			cols.push_back(COL_IS_TEMPLATE);
			cols.push_back(COL_PERIODSTART);
			addTableIndex(cols);

			addTableIndex(COL_FOLDER_ID);
		}
	}
}
