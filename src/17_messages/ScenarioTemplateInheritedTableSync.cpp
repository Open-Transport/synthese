
/** ScenarioTemplateInheritedTableSync class implementation.
	@file ScenarioTemplateInheritedTableSync.cpp

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

#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"

#include "17_messages/ScenarioInheritedTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;
	

	template<>
	const string util::FactorableTemplate<ScenarioTableSync, ScenarioTemplateInheritedTableSync>::FACTORY_KEY("ScenarioTemplateInheritedTableSync");

	namespace db
	{

		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,ScenarioTemplateInheritedTableSync,ScenarioTemplate>::Load(
			ScenarioTemplate* obj, 
			const SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);

			obj->setFolderId(rows->getLongLong(ScenarioTableSync::COL_FOLDER_ID));
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,ScenarioTemplateInheritedTableSync,ScenarioTemplate>::Unlink(ScenarioTemplate* obj, Env* env)
		{

		}


		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,ScenarioTemplateInheritedTableSync,ScenarioTemplate>::Save(ScenarioTemplate* obj)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;

			if (obj->getKey() == UNKNOWN_VALUE)
				obj->setKey(getId());

			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(obj->getKey())
				<< ",1"
				<< ",0" 
				<< "," << Conversion::ToSQLiteString(obj->getName())
				<< ",NULL"
				<< ",NULL"
				<< "," << Conversion::ToString(obj->getFolderId())
				<< ")";
			sqlite->execUpdate(query.str());
		}
	}

	namespace messages
	{

		ScenarioTemplateInheritedTableSync::ScenarioTemplateInheritedTableSync()
			: SQLiteInheritedNoSyncTableSyncTemplate<ScenarioTableSync, ScenarioTemplateInheritedTableSync, ScenarioTemplate>()
		{

		}



		void ScenarioTemplateInheritedTableSync::Search( util::Env& env, uid folderId , const std::string name /*= std::string() */, const ScenarioTemplate* scenarioToBeDifferentWith /*= NULL */, int first /*= 0 */, int number /*= -1 */, bool orderByName /*= true */, bool raisingOrder /*= false*/, util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */ )
		{
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
				<< COL_IS_TEMPLATE << "=1";
			if (folderId > 0)
				query << " AND " << COL_FOLDER_ID << "=" << Conversion::ToString(folderId);
			else if (folderId == 0)
				query << " AND (" << COL_FOLDER_ID << "=0 OR " << COL_FOLDER_ID << " IS NULL)";

			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToSQLiteString(name);
			if (scenarioToBeDifferentWith)
				query << " AND " << TABLE_COL_ID << "!=" << scenarioToBeDifferentWith->getKey();
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);
			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
