
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
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "TransportNetworkTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "LinkException.h"
#include "ReplaceQuery.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace db;
	using namespace util;
	using namespace env;
	
	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,NonConcurrencyRuleTableSync>::FACTORY_KEY("15.25.02 Non concurrency rules");
	}
	
	namespace pt
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
			// Properties
			object->setDelay(minutes(rows->getInt(NonConcurrencyRuleTableSync::COL_DELAY)));
			object->setHiddenLine(NULL);
			object->setPriorityLine(NULL);

			if(linkLevel < UP_LINKS_LOAD_LEVEL) return;

			try
			{
				object->setPriorityLine(CommercialLineTableSync::GetEditable(
						rows->getLongLong(NonConcurrencyRuleTableSync::COL_PRIORITY_LINE_ID),
						env
				).get()	);
			}
			catch(ObjectNotFoundException<CommercialLine>& e)
			{
				throw LinkException<NonConcurrencyRuleTableSync>(rows, NonConcurrencyRuleTableSync::COL_PRIORITY_LINE_ID, e);
			}

			try
			{
				object->setHiddenLine(CommercialLineTableSync::GetEditable(
					rows->getLongLong(NonConcurrencyRuleTableSync::COL_HIDDEN_LINE_ID),
					env
				).get()	);
			}
			catch(ObjectNotFoundException<CommercialLine>& e)
			{
				throw LinkException<NonConcurrencyRuleTableSync>(rows, NonConcurrencyRuleTableSync::COL_HIDDEN_LINE_ID, e);
			}

			if(linkLevel < ALGORITHMS_OPTIMIZATION_LOAD_LEVEL) return;

			object->getHiddenLine()->addConcurrencyRule(object);
		}



		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::Save(
			NonConcurrencyRule* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<NonConcurrencyRuleTableSync> query(*object);
			query.addField(object->getPriorityLine()->getKey());
			query.addField(object->getHiddenLine()->getKey());
			query.addField(object->getDelay().total_seconds() / 60);
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<NonConcurrencyRuleTableSync,NonConcurrencyRule>::Unlink(
			NonConcurrencyRule* obj
		){
			if(obj->getHiddenLine()) obj->getHiddenLine()->removeConcurrencyRule(obj);
		}
	}
	
	
	
	namespace pt
	{
		NonConcurrencyRuleTableSync::SearchResult NonConcurrencyRuleTableSync::Search(
			Env& env,
			optional<RegistryKeyType> hiddenLineId,
			optional<RegistryKeyType> priorityLineId
			, bool hiddenAndPriority
			, int first /*= 0*/
			, boost::optional<std::size_t> number  /*= 0*/ 
			, bool orderByPriorityLine //= true
			, bool orderByHiddenLine //= false
			, bool orderByDelay //= false
			, bool raisingOrder, //= true
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME;
			if(orderByPriorityLine)
			{
				query <<
					" INNER JOIN " << CommercialLineTableSync::TABLE.NAME << " c ON c." << TABLE_COL_ID << "=" << TABLE.NAME << "." << COL_PRIORITY_LINE_ID <<
					" INNER JOIN " << TransportNetworkTableSync::TABLE.NAME << " n ON n." << TABLE_COL_ID << "=c." << CommercialLineTableSync::COL_NETWORK_ID
				;
			}
			else if(orderByHiddenLine)
			{
				query <<
					" INNER JOIN " << CommercialLineTableSync::TABLE.NAME << " c ON c." << TABLE_COL_ID << "=" << TABLE.NAME << "." << COL_HIDDEN_LINE_ID <<
					" INNER JOIN " << TransportNetworkTableSync::TABLE.NAME << " n ON n." << TABLE_COL_ID << "=c." << CommercialLineTableSync::COL_NETWORK_ID
				;
			}
			query
				<< " WHERE 1 ";
			if (priorityLineId || hiddenLineId)
				query << " AND (";
			if (priorityLineId)
				query << COL_PRIORITY_LINE_ID << "=" << *priorityLineId;
			if (priorityLineId && hiddenLineId)
				query << (hiddenAndPriority ? " AND " : " OR ");
			if (hiddenLineId)
				query << COL_HIDDEN_LINE_ID << "=" << *hiddenLineId;
			if (priorityLineId || hiddenLineId)
				query << ")";

			if(orderByPriorityLine || orderByHiddenLine)
			{
				query <<
					" ORDER BY n." << TransportNetworkTableSync::COL_NAME << (raisingOrder ? " ASC" : " DESC") << "," <<
					" c." << CommercialLineTableSync::COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC")				
				;
			}
			else if(orderByDelay)
				query << " ORDER BY " << COL_DELAY << (raisingOrder ? " ASC" : " DESC");

			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
