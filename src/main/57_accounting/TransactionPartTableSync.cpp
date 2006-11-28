
#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/RegistryKeyException.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteException.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace accounts;

	namespace db
	{
		const std::string SQLiteTableSyncTemplate<TransactionPart>::TABLE_NAME = "t030_transaction_parts";
		const int SQLiteTableSyncTemplate<TransactionPart>::TABLE_ID = 30;
		const bool SQLiteTableSyncTemplate<TransactionPart>::HAS_AUTO_INCREMENT = true;
	}

	namespace accounts
	{
		const std::string TransactionPartTableSync::TABLE_COL_ID = "id";
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
			addTableColumn(TABLE_COL_LEFT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_RIGHT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_ACCOUNT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RATE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_TRADED_OBJECT_ID, "TEXT", true);
			addTableColumn(TABLE_COL_COMMENT, "TEXT", true);

		}

		void TransactionPartTableSync::loadTransactionPart( TransactionPart* tp, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			tp->setAccountId(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ACCOUNT_ID)));
			tp->setComment(rows.getColumn(rowId, TABLE_COL_COMMENT));
			tp->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			tp->setLeftCurrencyAmount(Conversion::ToDouble(rows.getColumn(rowId, TABLE_COL_LEFT_CURRENCY_AMOUNT)));
			tp->setRightCurrencyAmount(Conversion::ToDouble(rows.getColumn(rowId, TABLE_COL_RIGHT_CURRENCY_AMOUNT)));
			tp->setRateId(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_RATE_ID)));
			tp->setTradedObjectId(rows.getColumn(rowId, TABLE_COL_TRADED_OBJECT_ID));
		}

		void TransactionPartTableSync::saveTransactionPart( const db::SQLiteThreadExec* sqlite, TransactionPart* tp )
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

		void TransactionPartTableSync::rowsAdded( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionPartTableSync::rowsUpdated( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionPartTableSync::rowsRemoved( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}
	}
}