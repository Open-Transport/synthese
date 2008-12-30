
/** RollingStockTableSync class implementation.
	@file RollingStockTableSync.cpp
	@author Hugues Romain
	@date 2007

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

#include "RollingStockTableSync.h"

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
		template<> const string FactorableTemplate<SQLiteTableSync,RollingStockTableSync>::FACTORY_KEY("15.10.07 Rolling Stock");
	}

	namespace env
	{
		const string RollingStockTableSync::COL_NAME("name");
		const string RollingStockTableSync::COL_ARTICLE("article");
		const string RollingStockTableSync::COL_INDICATOR("indicator_label");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<RollingStockTableSync>::TABLE(
			"t049_rolling_stock"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<RollingStockTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RollingStockTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(RollingStockTableSync::COL_ARTICLE, SQL_TEXT),
			SQLiteTableSync::Field(RollingStockTableSync::COL_INDICATOR, SQL_TEXT),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<RollingStockTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<RollingStockTableSync,RollingStock>::Load(
			RollingStock* object
			, const db::SQLiteResultSPtr& rows
			, Env& env,
			LinkLevel linkLevel
		){
			// Properties
			object->setName(rows->getText(RollingStockTableSync::COL_NAME));
			object->setArticle(rows->getText(RollingStockTableSync::COL_ARTICLE));
			object->setIndicator(rows->getText(RollingStockTableSync::COL_INDICATOR));
		}



		template<> void SQLiteDirectTableSyncTemplate<RollingStockTableSync,RollingStock>::Save(
			RollingStock* object
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



		template<> void SQLiteDirectTableSyncTemplate<RollingStockTableSync,RollingStock>::Unlink(
			RollingStock* obj
		){
		}
	}
	
	
	
	namespace env
	{
		RollingStockTableSync::RollingStockTableSync()
			: SQLiteRegistryTableSyncTemplate<RollingStockTableSync,RollingStock>()
		{
		}



		void RollingStockTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
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

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
