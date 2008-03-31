
/** VinciStockAlertTableSync class implementation.
	@file VinciStockAlertTableSync.cpp
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

#include "VinciStockAlertTableSync.h"
#include "VinciStockAlert.h"

#include "57_accounting/TransactionPartTableSync.h"

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
	using namespace vinci;
	using namespace accounts;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,VinciStockAlertTableSync>::FACTORY_KEY("vinci_sock_alert");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<VinciStockAlertTableSync>::TABLE_NAME("t050_vinci_stock_alerts");
		template<> const int SQLiteTableSyncTemplate<VinciStockAlertTableSync>::TABLE_ID(50);
		template<> const bool SQLiteTableSyncTemplate<VinciStockAlertTableSync>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::load(
			VinciStockAlert* object
			, const db::SQLiteResultSPtr& rows
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setSiteId(rows->getLongLong(VinciStockAlertTableSync::COL_SITE_ID));
			object->setAccountId(rows->getLongLong(VinciStockAlertTableSync::COL_ACCOUNT_ID));
			object->setMinAlert(rows->getDouble(VinciStockAlertTableSync::COL_MIN_ALERT));
			object->setMaxAlert(rows->getDouble(VinciStockAlertTableSync::COL_MAX_ALERT));

			// Results
			try
			{
				object->setStockSize(rows->getDouble(VinciStockAlertTableSync::COL_STOCK_SIZE));
			}
			catch(...)
			{

			}
		}



		template<> void SQLiteDirectTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::save(
			VinciStockAlert* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << object->getSiteId()
				<< "," << object->getAccountId()
				<< "," << object->getMinAlert()
				<< "," << object->getMaxAlert()
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::_link(
			VinciStockAlert* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
		}


		template<> void SQLiteDirectTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::_unlink(
			VinciStockAlert* obj
		){
		}
	}
	
	
	
	namespace vinci
	{
		
		const string VinciStockAlertTableSync::COL_SITE_ID("site_id");
		const string VinciStockAlertTableSync::COL_ACCOUNT_ID("account_id");
		const string VinciStockAlertTableSync::COL_MIN_ALERT("min_alert");
		const string VinciStockAlertTableSync::COL_MAX_ALERT("max_alert");

		const string VinciStockAlertTableSync::COL_STOCK_SIZE("stock_size");


		VinciStockAlertTableSync::VinciStockAlertTableSync()
			: SQLiteNoSyncTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_SITE_ID, "INTEGER", false);
			addTableColumn(COL_ACCOUNT_ID, "INTEGER", false);
			addTableColumn(COL_MIN_ALERT, "REAL", true);
			addTableColumn(COL_MAX_ALERT, "REAL", true);

			addTableIndex(COL_SITE_ID);
			addTableIndex(COL_ACCOUNT_ID);
		}



		vector<shared_ptr<VinciStockAlert> > VinciStockAlertTableSync::search(
			uid siteId
			, uid accountId
			, int first /*= 0*/
			, int number /*= 0*/
			, bool searchError
		){
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< ", SUM(tp." << TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT << ") AS " << COL_STOCK_SIZE
				<< " FROM " << TABLE_NAME
				<< " LEFT JOIN " << TransactionPartTableSync::TABLE_NAME << " AS tp ON tp." << TransactionPartTableSync::COL_STOCK_ID << "=" << TABLE_NAME << "." << COL_SITE_ID << " AND tp." << TransactionPartTableSync::TABLE_COL_ACCOUNT_ID << "=" << TABLE_NAME << "." << COL_ACCOUNT_ID
				<< " WHERE 1 ";
			if (siteId != UNKNOWN_VALUE)
			 	query << " AND " << COL_SITE_ID << "=" << siteId;
			if (accountId != UNKNOWN_VALUE)
				query << " AND " << TABLE_NAME << "." << COL_ACCOUNT_ID << "=" << accountId;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			query << " GROUP BY " << TABLE_NAME << "." << TABLE_COL_ID;
			if (searchError)
				query << " HAVING " << COL_STOCK_SIZE << "<=" << TABLE_NAME << "." << COL_MIN_ALERT
					<< " OR " << COL_STOCK_SIZE << ">=" << TABLE_NAME << "." << COL_MAX_ALERT;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);
			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<VinciStockAlert> > objects;
				while (rows->next ())
				{
					shared_ptr<VinciStockAlert> object(new VinciStockAlert);
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
