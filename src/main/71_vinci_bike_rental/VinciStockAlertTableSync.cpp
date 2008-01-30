
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

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,VinciStockAlertTableSync>::FACTORY_KEY("vinci_sock_alert");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::TABLE_NAME("t050_vinci_stock_alerts");
		template<> const int SQLiteTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::TABLE_ID(50);
		template<> const bool SQLiteTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::load(
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
		}



		template<> void SQLiteTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::save(
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



		template<> void SQLiteTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::_link(
			VinciStockAlert* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
		}


		template<> void SQLiteTableSyncTemplate<VinciStockAlertTableSync,VinciStockAlert>::_unlink(
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
		){
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (siteId != UNKNOWN_VALUE)
			 	query << " AND " << COL_SITE_ID << "=" << siteId;
			if (accountId != UNKNOWN_VALUE)
				query << " AND " << COL_ACCOUNT_ID << "=" << accountId;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
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
