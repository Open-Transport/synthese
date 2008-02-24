
/** NonConcurrencyRuleTableSync class implementation.
	@file NonConcurrencyRuleTableSync.cpp
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

#include <sstream>

#include "NonConcurrencyRuleTableSync.h"
#include "NonConcurrencyRule.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,NonConcurrencyRuleTableSync>::FACTORY_KEY("15.25.02 Non concurrency rules");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<NonConcurrencyRuleTableSync>::TABLE_NAME("t052_non_concurrency_rules");
		template<> const int SQLiteTableSyncTemplate<NonConcurrencyRuleTableSync>::TABLE_ID(52);
		template<> const bool SQLiteTableSyncTemplate<NonConcurrencyRuleTableSync>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::load(
			NonConcurrencyRule* object
			, const db::SQLiteResultSPtr& rows
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			/// @todo Set all other attributes from the row

		}



		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::save(
			NonConcurrencyRule* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::_link(
			NonConcurrencyRule* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
			/// @todo Fill it
		}


		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::_unlink(
			NonConcurrencyRule* obj
		){
			/// @todo Fill it
		}
	}
	
	
	
	namespace env
	{
		const std::string NonConcurrencyRuleTableSync::COL_PRIORITY_LINE_ID("priority_line_id");
		const std::string NonConcurrencyRuleTableSync::COL_HIDDEN_LINE_ID("hidden_line_id");
		const std::string NonConcurrencyRuleTableSync::COL_DELAY("delay");



		NonConcurrencyRuleTableSync::NonConcurrencyRuleTableSync()
			: SQLiteRegistryTableSyncTemplate<NonConcurrencyRuleTableSync, NonConcurrencyRule>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_PRIORITY_LINE_ID, "INTEGER", false);
			addTableColumn(COL_HIDDEN_LINE_ID, "INTEGER", false);
			addTableColumn(COL_DELAY, "INTEGER", true);
		}

		vector<shared_ptr<NonConcurrencyRule> > NonConcurrencyRuleTableSync::search(
			int first /*= 0*/
			, int number /*= 0*/ 
			, bool orderByPriorityLine //= true
			, bool orderByHiddenLine //= false
			, bool orderByDelay //= false
			, bool raisingOrder //= true
		){
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<NonConcurrencyRule> > objects;
				while (rows->next ())
				{
					shared_ptr<NonConcurrencyRule> object(new NonConcurrencyRule);
					load(object.get(), rows);
					link(object.get(), rows, GET_AUTO);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
