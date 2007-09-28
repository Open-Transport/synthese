
/** TransactionPartTableSync class implementation.
	@file TransactionPartTableSync.cpp

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

#include "01_util/Conversion.h"
#include "01_util/RegistryKeyException.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "12_security/User.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Account.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace security;
	using namespace time;
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TransactionPart>::TABLE_NAME = "t030_transaction_parts";
		template<> const int SQLiteTableSyncTemplate<TransactionPart>::TABLE_ID = 30;
		template<> const bool SQLiteTableSyncTemplate<TransactionPart>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<TransactionPart>::load(TransactionPart* tp, const db::SQLiteResultSPtr& rows )
		{
			tp->setKey (rows->getLongLong (TABLE_COL_ID));
			tp->setTransactionId(rows->getLongLong ( TransactionPartTableSync::TABLE_COL_TRANSACTION_ID));
			tp->setAccountId(rows->getLongLong ( TransactionPartTableSync::TABLE_COL_ACCOUNT_ID));
			tp->setComment (rows->getText ( TransactionPartTableSync::TABLE_COL_COMMENT));
			tp->setLeftCurrencyAmount (rows->getDouble ( TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT));
			tp->setRightCurrencyAmount (rows->getDouble ( TransactionPartTableSync::TABLE_COL_RIGHT_CURRENCY_AMOUNT));
			tp->setRateId (rows->getLongLong ( TransactionPartTableSync::TABLE_COL_RATE_ID));
			tp->setTradedObjectId (rows->getText ( TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID));
		}


		template<> void SQLiteTableSyncTemplate<TransactionPart>::save(TransactionPart* tp)
		{
			try
			{
				SQLite* sqlite = DBModule::GetSQLite();
				if (!tp->getKey())
					tp->setKey(getId()); /// @todo Handle grid
				stringstream query;
				query << "REPLACE INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(tp->getKey())
					<< "," << Conversion::ToString(tp->getTransactionId())
					<< "," << Conversion::ToString(tp->getLeftCurrencyAmount())
					<< "," << Conversion::ToString(tp->getRightCurrencyAmount())
					<< "," << Conversion::ToString(tp->getAccountId())
					<< "," << Conversion::ToString(tp->getRateId())
					<< "," << Conversion::ToSQLiteString(tp->getTradedObjectId())
					<< "," << Conversion::ToSQLiteString(tp->getComment())
					<< ')';
				sqlite->execUpdate(query.str());
			}
			catch (SQLiteException& e)
			{

			}
		}


	}

	namespace accounts
	{
		const std::string TransactionPartTableSync::TABLE_COL_TRANSACTION_ID = "transaction_id";
		const std::string TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT = "left_currency_amount";
		const std::string TransactionPartTableSync::TABLE_COL_RIGHT_CURRENCY_AMOUNT = "right_currency_amount";
		const std::string TransactionPartTableSync::TABLE_COL_ACCOUNT_ID = "account_id";
		const std::string TransactionPartTableSync::TABLE_COL_RATE_ID = "rate_id";
		const std::string TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID = "traded_object";
		const std::string TransactionPartTableSync::TABLE_COL_COMMENT = "comment";

		TransactionPartTableSync::TransactionPartTableSync()
			: SQLiteTableSyncTemplate<TransactionPart>(true, true, TRIGGERS_ENABLED_CLAUSE, true)
		{
			// Columns
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_TRANSACTION_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_LEFT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_RIGHT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_ACCOUNT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RATE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_TRADED_OBJECT_ID, "TEXT", true);
			addTableColumn(TABLE_COL_COMMENT, "TEXT", true);

			// Indexes
			addTableIndex(TABLE_COL_TRANSACTION_ID);
			addTableIndex(TABLE_COL_ACCOUNT_ID);
			addTableIndex(TABLE_COL_TRADED_OBJECT_ID);
		}

		void TransactionPartTableSync::rowsAdded( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{

		}

		void TransactionPartTableSync::rowsUpdated( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{

		}

		void TransactionPartTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{

		}

		vector<shared_ptr<TransactionPart> > TransactionPartTableSync::search(
				shared_ptr<const Transaction> transaction, shared_ptr<const Account> account
				, int first, int number)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT * "
				<< " FROM " << TABLE_NAME << " AS p "
				<< " WHERE "
				<< " p." << TABLE_COL_TRANSACTION_ID << "=" << Conversion::ToString(transaction->getKey());
			if (account != NULL)
				query << " AND p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey());
			query << " LIMIT " << number << " OFFSET " << first;
			
			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			vector<shared_ptr<TransactionPart> > tps;
			while (rows->next ())
			{
			    shared_ptr<TransactionPart> tp(new TransactionPart);
			    load(tp.get(), rows);
			    tps.push_back(tp);
			}
			return tps;
		}


		vector<shared_ptr<TransactionPart> > TransactionPartTableSync::search(
			shared_ptr<const Account> account
			, shared_ptr<const User> user
			, bool order, int first /*= 0*/, int number /*= 0*/
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT * "
				<< " FROM " << TABLE_NAME << " AS p "
				<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " WHERE "
				<< " p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
				;
			if (user != NULL)
				query << " AND t." << TransactionTableSync::TABLE_COL_LEFT_USER_ID << "=" << Conversion::ToString(user->getKey());
			query << " ORDER BY " << TransactionTableSync::TABLE_COL_START_DATE_TIME;
			if (!order)
				query << " DESC ";
			query << " LIMIT " << number << " OFFSET " << first;
			
			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			vector<shared_ptr<TransactionPart> > tps;
			while (rows->next ())
			{
			    shared_ptr<TransactionPart> tp(new TransactionPart);
			    load(tp.get (), rows);
			    tps.push_back(tp);
			}
			return tps;
		}
	    
	    
		map<int, int> TransactionPartTableSync::count(
			shared_ptr<const Account> account, Date startDate, Date endDate, int first, int number
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT strftime('%H', t.start_date_time) AS hours,"
					<< " count(t." << TABLE_COL_ID << ") AS numbers "
				<< " FROM " << TABLE_NAME << " AS p "
					<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " WHERE "
					<< " p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << "<=" << endDate.toSQLString()
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << ">=" << startDate.toSQLString()
				<< " GROUP BY strftime('%H', t.start_date_time) "
				<< " LIMIT " << number << " OFFSET " << first
			;
			
			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			map<int, int> mapii;
			while (rows->next ())
			{
			    mapii.insert (make_pair(rows->getInt("hours"), rows->getInt ("numbers")));
			}
			return mapii;
		}

	}
}
