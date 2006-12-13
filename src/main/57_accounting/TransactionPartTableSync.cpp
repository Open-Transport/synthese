
#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/RegistryKeyException.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteException.h"

#include "12_security/User.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Account.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace security;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TransactionPart>::TABLE_NAME = "t030_transaction_parts";
		template<> const int SQLiteTableSyncTemplate<TransactionPart>::TABLE_ID = 30;
		template<> const bool SQLiteTableSyncTemplate<TransactionPart>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<TransactionPart>::load(TransactionPart* tp, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			tp->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			tp->setTransactionId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_TRANSACTION_ID)));
			tp->setAccountId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_ACCOUNT_ID)));
			tp->setComment(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_COMMENT));
			tp->setLeftCurrencyAmount(Conversion::ToDouble(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT)));
			tp->setRightCurrencyAmount(Conversion::ToDouble(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_RIGHT_CURRENCY_AMOUNT)));
			tp->setRateId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_RATE_ID)));
			tp->setTradedObjectId(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID));
		}

		template<> void SQLiteTableSyncTemplate<TransactionPart>::save(const db::SQLiteThreadExec* sqlite, TransactionPart* tp)
		{
			try
			{
				if (tp->getKey() > 0)
				{
					/// @todo UPDATE implementation
				}
				else
				{
					tp->setKey(getId(1,1)); /// @todo Handle grid
					stringstream query;
					query << "INSERT INTO " << TABLE_NAME << " VALUES("
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
			}
			catch (SQLiteException& e)
			{

			}
		}


	}

	namespace accounts
	{
		const std::string TransactionPartTableSync::TABLE_COL_ID = "id";
		const std::string TransactionPartTableSync::TABLE_COL_TRANSACTION_ID = "transaction_id";
		const std::string TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT = "left_currency_amount";
		const std::string TransactionPartTableSync::TABLE_COL_RIGHT_CURRENCY_AMOUNT = "right_currency_amount";
		const std::string TransactionPartTableSync::TABLE_COL_ACCOUNT_ID = "account_id";
		const std::string TransactionPartTableSync::TABLE_COL_RATE_ID = "rate_id";
		const std::string TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID = "traded_object";
		const std::string TransactionPartTableSync::TABLE_COL_COMMENT = "comment";

		TransactionPartTableSync::TransactionPartTableSync()
			: SQLiteTableSyncTemplate<TransactionPart>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_TRANSACTION_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_LEFT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_RIGHT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_ACCOUNT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RATE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_TRADED_OBJECT_ID, "TEXT", true);
			addTableColumn(TABLE_COL_COMMENT, "TEXT", true);

		}

		void TransactionPartTableSync::rowsAdded( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionPartTableSync::rowsUpdated( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionPartTableSync::rowsRemoved( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		vector<TransactionPart*> TransactionPartTableSync::searchTransactionParts( const db::SQLiteThreadExec* sqlite , Account* account, User* user , int first /*= 0*/, int number /*= 0*/ )
		{
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
			query << " LIMIT " << number << " OFFSET " << first;
			
			SQLiteResult result = sqlite->execQuery(query.str());
			vector<TransactionPart*> tps;
			for (int i=0; i<result.getNbRows(); ++i)
			{
//				try
//				{
					TransactionPart* tp = new TransactionPart;
					load(tp, result, i);
					tps.push_back(tp);
//				}
			}
			return tps;
		}
	}
}
