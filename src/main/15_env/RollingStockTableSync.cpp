
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
		template<> const string FactorableTemplate<SQLiteTableSync,RollingStockTableSync>::FACTORY_KEY("15.10.07 Rolling Stock");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<RollingStockTableSync,RollingStock>::TABLE_NAME("t049_rolling_stock");
		template<> const int SQLiteTableSyncTemplate<RollingStockTableSync,RollingStock>::TABLE_ID(49);
		template<> const bool SQLiteTableSyncTemplate<RollingStockTableSync,RollingStock>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteTableSyncTemplate<RollingStockTableSync,RollingStock>::load(
			RollingStock* object
			, const db::SQLiteResultSPtr& rows
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setName(rows->getText(RollingStockTableSync::COL_NAME));
			object->setArticle(rows->getText(RollingStockTableSync::COL_ARTICLE));
			object->setIndicator(rows->getText(RollingStockTableSync::COL_INDICATOR));
		}



		template<> void SQLiteTableSyncTemplate<RollingStockTableSync,RollingStock>::save(
			RollingStock* object
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



		template<> void SQLiteTableSyncTemplate<RollingStockTableSync,RollingStock>::_link(
			RollingStock* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
			/// @todo Fill it
		}


		template<> void SQLiteTableSyncTemplate<RollingStockTableSync,RollingStock>::_unlink(
			RollingStock* obj
		){
			/// @todo Fill it
		}
	}
	
	
	
	namespace env
	{
		const std::string RollingStockTableSync::COL_NAME("name");
		const std::string RollingStockTableSync::COL_ARTICLE("article");
		const std::string RollingStockTableSync::COL_INDICATOR("indicator_label");
		

		RollingStockTableSync::RollingStockTableSync()
			: SQLiteRegistryTableSyncTemplate<RollingStockTableSync,RollingStock>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_NAME, "TEXT");
			addTableColumn(COL_ARTICLE, "TEXT");
			addTableColumn(COL_INDICATOR, "TEXT");
		}



		vector<shared_ptr<RollingStock> > RollingStockTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
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
				vector<shared_ptr<RollingStock> > objects;
				while (rows->next ())
				{
					shared_ptr<RollingStock> object(new RollingStock);
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
