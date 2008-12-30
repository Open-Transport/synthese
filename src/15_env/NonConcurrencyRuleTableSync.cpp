
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

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

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
	
	namespace env
	{
		const std::string NonConcurrencyRuleTableSync::COL_PRIORITY_LINE_ID("priority_line_id");
		const std::string NonConcurrencyRuleTableSync::COL_HIDDEN_LINE_ID("hidden_line_id");
		const std::string NonConcurrencyRuleTableSync::COL_DELAY("delay");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<NonConcurrencyRuleTableSync>::TABLE(
			"t056_non_concurrency_rules"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<NonConcurrencyRuleTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(NonConcurrencyRuleTableSync::COL_PRIORITY_LINE_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(NonConcurrencyRuleTableSync::COL_HIDDEN_LINE_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(NonConcurrencyRuleTableSync::COL_DELAY, SQL_INTEGER),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<NonConcurrencyRuleTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::Load(
			NonConcurrencyRule* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Columns reading
			uid hiddenLineId(rows->getLongLong(NonConcurrencyRuleTableSync::COL_HIDDEN_LINE_ID));
			uid priorityLineId(rows->getLongLong(NonConcurrencyRuleTableSync::COL_PRIORITY_LINE_ID));
			int delay(rows->getInt(NonConcurrencyRuleTableSync::COL_DELAY));

			// Properties
			object->setDelay(delay);
			object->setHiddenLine(hiddenLineId);
			object->setPriorityLine(priorityLineId);
		}



		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::Save(
			NonConcurrencyRule* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::Unlink(
			NonConcurrencyRule* obj
		){
			
		}
	}
	
	
	
	namespace env
	{
		NonConcurrencyRuleTableSync::NonConcurrencyRuleTableSync()
			: SQLiteRegistryTableSyncTemplate<NonConcurrencyRuleTableSync, NonConcurrencyRule>()
		{
		}

		void NonConcurrencyRuleTableSync::Search(
			Env& env,
			util::RegistryKeyType hiddenLineId
			, util::RegistryKeyType priorityLineId
			, bool hiddenAndPriority
			, int first /*= 0*/
			, int number /*= 0*/ 
			, bool orderByPriorityLine //= true
			, bool orderByHiddenLine //= false
			, bool orderByDelay //= false
			, bool raisingOrder, //= true
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (priorityLineId != UNKNOWN_VALUE && hiddenLineId != UNKNOWN_VALUE)
				query << " AND (";
			if (priorityLineId != UNKNOWN_VALUE)
				query << COL_PRIORITY_LINE_ID << "=" << priorityLineId;
			if (priorityLineId != UNKNOWN_VALUE && hiddenLineId != UNKNOWN_VALUE)
				query << (hiddenAndPriority ? " AND " : " OR ");
			if (hiddenLineId != UNKNOWN_VALUE)
				query << COL_HIDDEN_LINE_ID << "=" << hiddenLineId;
			if (priorityLineId != UNKNOWN_VALUE && hiddenLineId != UNKNOWN_VALUE)
				query << ")";

			if(orderByPriorityLine)
				query << " ORDER BY " << COL_PRIORITY_LINE_ID << (raisingOrder ? " ASC" : " DESC");
			else if(orderByHiddenLine)
				query << " ORDER BY " << COL_HIDDEN_LINE_ID << (raisingOrder ? " ASC" : " DESC");
			else if(orderByDelay)
				query << " ORDER BY " << COL_DELAY << (raisingOrder ? " ASC" : " DESC");

			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
