
/** VinciContractTableSync class implementation.
	@file VinciContractTableSync.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserTableSyncException.h"

#include "57_accounting/TransactionTableSync.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace security;
	using namespace vinci;
	using namespace accounts;
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciContract>::TABLE_NAME = "t035_vinci_contract";
		template<> const int SQLiteTableSyncTemplate<VinciContract>::TABLE_ID = 35;
		template<> const bool SQLiteTableSyncTemplate<VinciContract>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciContract>::load(VinciContract* vc, const SQLiteResultSPtr& rows, int rowId)
		{
			vc->setKey(rows->getLongLong (TABLE_COL_ID));
			vc->setUserId(rows->getLongLong ( VinciContractTableSync::COL_USER_ID)));
			vc->setSiteId(rows->getLongLong ( VinciContractTableSync::COL_SITE_ID)));
			vc->setPassport(rows->get ( VinciContractTableSync::COL_PASSPORT));
		}

		template<> void SQLiteTableSyncTemplate<VinciContract>::save(VinciContract* vc)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (vc->getKey() <= 0)
				vc->setKey(getId()); /// @todo Handle grid ID
			
			// INSERT
			query << "REPLACE INTO " << TABLE_NAME << " VALUES("
					<< vc->getKey()
					<< "," << Conversion::ToString(vc->getUserId())
					<< "," << Conversion::ToString(vc->getSiteId())
					<< "," << vc->getDate().toSQLString()
					<< "," << Conversion::ToSQLiteString(vc->getPassport())
					<< ")";
			sqlite->execUpdate(query.str());
		}
	}

	namespace vinci
	{
		const std::string VinciContractTableSync::COL_USER_ID = "user_id";
		const std::string VinciContractTableSync::COL_SITE_ID = "site_id";
		const std::string VinciContractTableSync::COL_DATE = "date";
		const std::string VinciContractTableSync::COL_PASSPORT = "passport";

		VinciContractTableSync::VinciContractTableSync()
			: SQLiteTableSyncTemplate<VinciContract>(true, true, TRIGGERS_ENABLED_CLAUSE, true)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_USER_ID, "INTEGER", true);
			addTableColumn(COL_SITE_ID, "INTEGER", true);
			addTableColumn(COL_DATE, "TIMESTAMP", true);
			addTableColumn(COL_PASSPORT, "TEXT", true);

			addTableIndex(COL_USER_ID);
		}

		void VinciContractTableSync::rowsAdded( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{

		}

		void VinciContractTableSync::rowsUpdated( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{

		}

		void VinciContractTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{

		}

		std::vector<shared_ptr<VinciContract> > VinciContractTableSync::search(
			std::string name /*= ""*/
			, std::string surname /*= "" */
			, int first /*= 0*/
			, int number /*=-1*/ 
			, bool orderByNameAndSurname
			, bool orderBySurnameAndName
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< "SELECT *"
				<< " FROM "
					<< TABLE_NAME << " AS c "
					<< " INNER JOIN " << UserTableSync::TABLE_NAME << " AS u ON c." << COL_USER_ID << "=u." << TABLE_COL_ID
				<< " WHERE "
					<< "u." << UserTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
					<< " AND u." << UserTableSync::TABLE_COL_SURNAME << " LIKE '%" << Conversion::ToSQLiteString(surname, false) << "%'";
			if (orderByNameAndSurname)
				query << " ORDER BY " << UserTableSync::TABLE_COL_NAME << "," << UserTableSync::TABLE_COL_SURNAME << (raisingOrder ? " ASC" : " DESC");
			if (orderBySurnameAndName)
				query << " ORDER BY " << UserTableSync::TABLE_COL_SURNAME << "," << UserTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (first)
				query << " OFFSET " << first;
			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciContract> > contracts;
			for (int i=0; i<result.getNbRows(); ++i)
			{
				try
				{
					shared_ptr<VinciContract> contract(new VinciContract);
					SQLiteTableSyncTemplate<VinciContract>::load(contract.get (), result, i);
					contract->getUser();
					contracts.push_back(contract);
				}
				catch (UserTableSyncException e)
				{
					
				}
			}
			return contracts;
		}
	}
}
