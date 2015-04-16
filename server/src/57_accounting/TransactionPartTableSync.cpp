
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

#include "Conversion.h"

#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"

#include "User.h"

#include "AccountingModule.h"
#include "Account.h"
#include "Transaction.h"
#include "TransactionTableSync.h"
#include "TransactionPart.h"
#include "TransactionPartTableSync.h"
#include "Account.h"
#include "AccountTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBTableSync,TransactionPartTableSync>::FACTORY_KEY("57.30 Transaction Part");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TransactionPartTableSync>::TABLE.NAME = "t030_transaction_parts";
		template<> const int DBTableSyncTemplate<TransactionPartTableSync>::TABLE.ID = 30;
		template<> const bool DBTableSyncTemplate<TransactionPartTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void DBDirectTableSyncTemplate<TransactionPartTableSync,TransactionPart>::load(TransactionPart* tp, const db::DBResultSPtr& rows )
		{
			tp->setKey (rows->getLongLong (TABLE_COL_ID));
			tp->setTransactionId(rows->getLongLong ( TransactionPartTableSync::TABLE_COL_TRANSACTION_ID));
			tp->setAccountId(rows->getLongLong ( TransactionPartTableSync::TABLE_COL_ACCOUNT_ID));
			tp->setComment (rows->getText ( TransactionPartTableSync::TABLE_COL_COMMENT));
			tp->setLeftCurrencyAmount (rows->getDouble ( TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT));
			tp->setRightCurrencyAmount (rows->getDouble ( TransactionPartTableSync::TABLE_COL_RIGHT_CURRENCY_AMOUNT));
			tp->setRateId (rows->getLongLong ( TransactionPartTableSync::TABLE_COL_RATE_ID));
			tp->setTradedObjectId (rows->getText ( TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID));
			tp->setStockId(rows->getLongLong(TransactionPartTableSync::COL_STOCK_ID));
		}


		template<> void DBDirectTableSyncTemplate<TransactionPartTableSync,TransactionPart>::_link(TransactionPart* tp, const db::DBResultSPtr& rows, GetSource temporary )
		{

		}



		template<> void DBDirectTableSyncTemplate<TransactionPartTableSync,TransactionPart>::_unlink(TransactionPart* tp)
		{

		}


		template<> void DBDirectTableSyncTemplate<TransactionPartTableSync,TransactionPart>::Save(TransactionPart* tp)
		{
			try
			{
				// TODO: use ReplaceQuery instead of building the SQL.
				DB* db = DBModule::GetDB();
				if (!tp->getKey())
					tp->setKey(getId());
				stringstream query;
				query << "REPLACE INTO " << TABLE.NAME << " VALUES("
					<< Conversion::ToString(tp->getKey())
					<< "," << Conversion::ToString(tp->getTransactionId())
					<< "," << Conversion::ToString(tp->getLeftCurrencyAmount())
					<< "," << Conversion::ToString(tp->getRightCurrencyAmount())
					<< "," << Conversion::ToString(tp->getAccountId())
					<< "," << Conversion::ToString(tp->getRateId())
					<< "," << Conversion::ToDBString(tp->getTradedObjectId())
					<< "," << Conversion::ToDBString(tp->getComment())
					<< "," << Conversion::ToString(tp->getStockId())
					<< ')';
				db->execUpdate(query.str());
			}
			catch (DBException& e)
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
		const string TransactionPartTableSync::COL_STOCK_ID("stock_id");

		TransactionPartTableSync::TransactionPartTableSync()
			: DBNoSyncTableSyncTemplate<TransactionPartTableSync,TransactionPart>()
		{
			// Columns
			addTableColumn(TABLE_COL_ID, "SQL_INTEGER", false);
			addTableColumn(TABLE_COL_TRANSACTION_ID, "SQL_INTEGER", true);
			addTableColumn(TABLE_COL_LEFT_CURRENCY_AMOUNT, "SQL_REAL", true);
			addTableColumn(TABLE_COL_RIGHT_CURRENCY_AMOUNT, "SQL_REAL", true);
			addTableColumn(TABLE_COL_ACCOUNT_ID, "SQL_INTEGER", true);
			addTableColumn(TABLE_COL_RATE_ID, "SQL_INTEGER", true);
			addTableColumn(TABLE_COL_TRADED_OBJECT_ID, "SQL_TEXT", true);
			addTableColumn(TABLE_COL_COMMENT, "SQL_TEXT", true);
			addTableColumn(COL_STOCK_ID, "SQL_INTEGER", true);

			// Indexes
			addTableIndex(TABLE_COL_TRANSACTION_ID);
			addTableIndex(TABLE_COL_ACCOUNT_ID);
			addTableIndex(TABLE_COL_TRADED_OBJECT_ID);
			addTableIndex(COL_STOCK_ID);
		}



		vector<shared_ptr<TransactionPart> > TransactionPartTableSync::search(
				shared_ptr<const Transaction> transaction, shared_ptr<const Account> account
				, int first, int number)
		{
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< " SELECT * "
				<< " FROM " << TABLE.NAME << " AS p "
				<< " WHERE "
				<< " p." << TABLE_COL_TRANSACTION_ID << "=" << Conversion::ToString(transaction->getKey());
			if (account != NULL)
				query << " AND p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey());
			query << " LIMIT " << number << " OFFSET " << first;

			DBResultSPtr rows = db->execQuery(query.str());
			vector<shared_ptr<TransactionPart> > tps;
			while (rows->next ())
			{
			    shared_ptr<TransactionPart> tp(new TransactionPart);
			    load(tp.get(), rows);
			    tps.push_back(tp);
			}
			return tps;
		}



		vector<shared_ptr<TransactionPart> > TransactionPartTableSync::Search(
			string accountCode
			, uid rightUserId
			, uid stockId
			, bool orderByAccount
			, bool orderByDate
			, bool raisingOrder
			, int first
			, int number
		){
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< " SELECT * "
				<< " FROM " << TABLE.NAME << " AS p "
				<< " INNER JOIN " << TransactionTableSync::TABLE.NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " INNER JOIN " << AccountTableSync::TABLE.NAME << " AS a ON a." << TABLE_COL_ID << "=p." << TABLE_COL_ACCOUNT_ID
				<< " WHERE "
				<< " a." << AccountTableSync::TABLE_COL_RIGHT_CLASS_NUMBER << " LIKE " << Conversion::ToDBString(accountCode)
			;
			if (rightUserId != UNKNOWN_VALUE)
				query << " AND a." << AccountTableSync::TABLE_COL_RIGHT_USER_ID << "=" << rightUserId;
			if (stockId != UNKNOWN_VALUE)
				query << " AND p." << COL_STOCK_ID << "=" << stockId;
			if (orderByAccount)
				query << " ORDER BY a." << AccountTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << ",t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << " DESC";
			if (orderByDate)
				query << " ORDER BY t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << (raisingOrder ? " ASC" : " DESC");
			query << " LIMIT " << number << " OFFSET " << first;

			DBResultSPtr rows = db->execQuery(query.str());
			vector<shared_ptr<TransactionPart> > tps;
			while (rows->next ())
			{
				shared_ptr<TransactionPart> tp(new TransactionPart);
				load(tp.get (), rows);
				tps.push_back(tp);
			}
			return tps;
		}


		vector<shared_ptr<TransactionPart> > TransactionPartTableSync::search(
			shared_ptr<const Account> account
			, shared_ptr<const User> user
			, bool order, int first /*= 0*/, int number /*= 0*/
		){
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< " SELECT * "
				<< " FROM " << TABLE.NAME << " AS p "
				<< " INNER JOIN " << TransactionTableSync::TABLE.NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " WHERE "
				<< " p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
			;
			query << " AND t." << TransactionTableSync::TABLE_COL_LEFT_USER_ID << "=" << Conversion::ToString(user.get() ? user->getKey() : 0);
			query << " ORDER BY " << TransactionTableSync::TABLE_COL_START_DATE_TIME;
			if (!order)
				query << " DESC ";
			query << " LIMIT " << number << " OFFSET " << first;

			DBResultSPtr rows = db->execQuery(query.str());
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
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< " SELECT strftime('%H', t.start_date_time) AS hours,"
					<< " count(t." << TABLE_COL_ID << ") AS numbers "
				<< " FROM " << TABLE.NAME << " AS p "
					<< " INNER JOIN " << TransactionTableSync::TABLE.NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " WHERE "
					<< " p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << "<=" << endDate.toSQLString()
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << ">=" << startDate.toSQLString()
				<< " GROUP BY strftime('%H', t.start_date_time) "
				<< " LIMIT " << number << " OFFSET " << first
			;

			DBResultSPtr rows = db->execQuery(query.str());
			map<int, int> mapii;
			while (rows->next ())
			{
			    mapii.insert (make_pair(rows->getInt("hours"), rows->getInt ("numbers")));
			}
			return mapii;
		}

		double TransactionPartTableSync::sum(
			boost::shared_ptr<const Account> account
			, boost::shared_ptr<const security::User> user
		){
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< " SELECT sum(p." << TABLE_COL_LEFT_CURRENCY_AMOUNT << ") AS sum"
				<< " FROM " << TABLE.NAME << " AS p "
				<< " INNER JOIN " << TransactionTableSync::TABLE.NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " WHERE "
				<< " p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
				<< " AND t." << TransactionTableSync::TABLE_COL_LEFT_USER_ID << "=" << Conversion::ToString(user.get() ? user->getKey() : 0)
				;

			DBResultSPtr rows = db->execQuery(query.str());
			return rows->next () ? rows->getDouble("sum") : 0;
		}
	}
}
