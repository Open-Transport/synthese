
/** AlarmTemplateInheritedTableSync class implementation.
	@file AlarmTemplateInheritedTableSync.cpp

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

#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include <sstream>

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;

	template<>
	const string util::FactorableTemplate<AlarmTableSync, AlarmTemplateInheritedTableSync>::FACTORY_KEY("AlarmTemplateInheritedTableSync");

	namespace db
	{

		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,AlarmTemplateInheritedTableSync,AlarmTemplate>::Load(
			AlarmTemplate* obj,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				obj->setScenario(
					ScenarioTemplateInheritedTableSync::Get(
						rows->getLongLong(AlarmTableSync::COL_SCENARIO_ID),
						env, linkLevel
					).get()
				);
			}
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,AlarmTemplateInheritedTableSync,AlarmTemplate>::Unlink(
			AlarmTemplate* obj
		){
			obj->setScenario(NULL);
		}



		template<>
		void SQLiteInheritedTableSyncTemplate<AlarmTableSync,AlarmTemplateInheritedTableSync,AlarmTemplate>::Save(
			AlarmTemplate* obj
		){
			stringstream query;
			if (obj->getKey() == UNKNOWN_VALUE)
				obj->setKey(getId());
			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(obj->getKey())
				<< ",1"
				<< ",0"
				<< "," << Conversion::ToString((int) obj->getLevel())
				<< "," << Conversion::ToSQLiteString(obj->getShortMessage())
				<< "," << Conversion::ToSQLiteString(obj->getLongMessage())
				<< ",''"
				<< ",''"
				<< "," << (obj->getScenario() ? Conversion::ToString(obj->getScenario()->getKey()) : "")
				<< ",0"
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}

	namespace messages
	{

		AlarmTemplateInheritedTableSync::AlarmTemplateInheritedTableSync()
			: SQLiteInheritedNoSyncTableSyncTemplate<AlarmTableSync, AlarmTemplateInheritedTableSync, AlarmTemplate>()
		{

		}



		void AlarmTemplateInheritedTableSync::Search(
			Env& env,
			RegistryKeyType scenarioId,
			int first /*= 0 */,
			int number /*= 0 */,
			bool orderByLevel /*= false */,
			bool raisingOrder /*= false*/,
			LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE.NAME << " AS a "
				<< " WHERE "
				<< COL_IS_TEMPLATE << "=1"
				<< " AND " << COL_SCENARIO_ID << "=" << scenarioId;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);
			
			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
