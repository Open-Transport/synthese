
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

#include "ScenarioTableSync.h"
#include "SentScenario.h"
#include "SentScenarioInheritedTableSync.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "AlarmTableSync.h"
#include "Conversion.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "DateTime.h"

#include <sstream>

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

	namespace messages
	{
		const string ScenarioTableSync::COL_IS_TEMPLATE = "is_template";
		const string ScenarioTableSync::COL_ENABLED = "is_enabled";
		const string ScenarioTableSync::COL_NAME = "name";
		const string ScenarioTableSync::COL_PERIODSTART = "period_start";
		const string ScenarioTableSync::COL_PERIODEND = "period_end"; 
		const string ScenarioTableSync::COL_FOLDER_ID("folder_id");
	}

	namespace db
	{
		template<> const SQLiteTableFormat SQLiteTableSyncTemplate<ScenarioTableSync>::TABLE(
			ScenarioTableSync::CreateFormat(
				"t039_scenarios",
				SQLiteTableFormat::CreateFields(
					SQLiteTableFormat::Field(ScenarioTableSync::COL_IS_TEMPLATE, INTEGER),
					SQLiteTableFormat::Field(ScenarioTableSync::COL_ENABLED, INTEGER),
					SQLiteTableFormat::Field(ScenarioTableSync::COL_NAME, TEXT),
					SQLiteTableFormat::Field(ScenarioTableSync::COL_PERIODSTART, TIMESTAMP),
					SQLiteTableFormat::Field(ScenarioTableSync::COL_PERIODEND, TIMESTAMP),
					SQLiteTableFormat::Field(ScenarioTableSync::COL_FOLDER_ID, INTEGER),
					SQLiteTableFormat::Field()
				), SQLiteTableFormat::CreateIndexes(
					SQLiteTableFormat::Index(
						"templateperiod",
						SQLiteTableFormat::Index::CreateFieldsList(
							ScenarioTableSync::COL_IS_TEMPLATE,
							ScenarioTableSync::COL_PERIODSTART,
							string()
					)	),
					SQLiteTableFormat::Index(ScenarioTableSync::COL_FOLDER_ID),
					SQLiteTableFormat::Index()
		)	)	);


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
		ScenarioTableSync::ScenarioTableSync()
			: SQLiteInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>()
		{
		}
	}
}
