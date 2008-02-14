
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

#include "04_time/DateTime.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserTableSyncException.h"

#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/Account.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace security;
	using namespace vinci;
	using namespace accounts;
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,VinciContractTableSync>::FACTORY_KEY("71.11 Vinci Contract");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciContractTableSync>::TABLE_NAME = "t035_vinci_contract";
		template<> const int SQLiteTableSyncTemplate<VinciContractTableSync>::TABLE_ID = 35;
		template<> const bool SQLiteTableSyncTemplate<VinciContractTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<VinciContractTableSync,VinciContract>::load(VinciContract* vc, const SQLiteResultSPtr& rows)
		{
			vc->setKey(rows->getLongLong (TABLE_COL_ID));
			vc->setUserId(rows->getLongLong ( VinciContractTableSync::COL_USER_ID));
			vc->setSiteId(rows->getLongLong ( VinciContractTableSync::COL_SITE_ID));
			vc->setPassport(rows->getText ( VinciContractTableSync::COL_PASSPORT));
		}

		template<> void SQLiteDirectTableSyncTemplate<VinciContractTableSync,VinciContract>::_link(VinciContract* vc, const SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<VinciContractTableSync,VinciContract>::_unlink(VinciContract* vc)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<VinciContractTableSync,VinciContract>::save(VinciContract* vc)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (vc->getKey() <= 0)
				vc->setKey(getId());
			
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
			: SQLiteNoSyncTableSyncTemplate<VinciContractTableSync,VinciContract>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_USER_ID, "INTEGER", true);
			addTableColumn(COL_SITE_ID, "INTEGER", true);
			addTableColumn(COL_DATE, "TIMESTAMP", true);
			addTableColumn(COL_PASSPORT, "TEXT", true);

			addTableIndex(COL_USER_ID);
		}



		std::vector<shared_ptr<VinciContract> > VinciContractTableSync::search(
			std::string name /*= ""*/
			, std::string surname /*= "" */
			, int first /*= 0*/
			, int number /*=-1*/ 
			, bool orderByNameAndSurname
			, bool orderBySurnameAndName
			, bool orderByLate
			, bool raisingOrder
		){
			DateTime yesterday(TIME_CURRENT);
			yesterday -= 1;
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< "SELECT *"
				<< ",(SELECT MAX(t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << ") FROM " 
					<< TransactionPartTableSync::TABLE_NAME << " AS p INNER JOIN " 
					<< TransactionTableSync::TABLE_NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID
					<< " WHERE t." << TransactionTableSync::TABLE_COL_LEFT_USER_ID << "=u." << TABLE_COL_ID
					<< " AND p." << TransactionPartTableSync::TABLE_COL_ACCOUNT_ID << "=" << VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE)->getKey()
					<< " AND t." << TransactionTableSync::TABLE_COL_END_DATE_TIME << " IS NULL "
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << "<" << yesterday.toSQLString()
					<< ") AS ret"
				<< " FROM "
					<< TABLE_NAME << " AS c "
					<< " INNER JOIN " << UserTableSync::TABLE_NAME << " AS u ON c." << COL_USER_ID << "=u." << TABLE_COL_ID
				<< " WHERE "
					<< "u." << UserTableSync::TABLE_COL_NAME << " LIKE '" << Conversion::ToSQLiteString(name, false) << "%'"
					<< " AND u." << UserTableSync::TABLE_COL_SURNAME << " LIKE '" << Conversion::ToSQLiteString(surname, false) << "%'";
			if (orderByNameAndSurname)
				query << " ORDER BY " << UserTableSync::TABLE_COL_NAME << "," << UserTableSync::TABLE_COL_SURNAME << (raisingOrder ? " ASC" : " DESC");
			if (orderBySurnameAndName)
				query << " ORDER BY " << UserTableSync::TABLE_COL_SURNAME << "," << UserTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByLate)
				query << " ORDER BY ret" << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (first)
				query << " OFFSET " << first;
			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciContract> > contracts;
			while(rows->next())
			{
				try
				{
					shared_ptr<VinciContract> contract(new VinciContract);
					SQLiteDirectTableSyncTemplate<VinciContractTableSync,VinciContract>::load(contract.get (), rows);
					contract->getUser();
					DateTime late(DateTime::FromSQLTimestamp(rows->getText("ret")));
					contract->setLate(late);
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
