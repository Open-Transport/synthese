
#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/RegistryKeyException.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace time;
	using namespace accounts;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Transaction>::TABLE_NAME = "t031_transactions";
		template<> const int SQLiteTableSyncTemplate<Transaction>::TABLE_ID = 31;
		template<> const bool SQLiteTableSyncTemplate<Transaction>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Transaction>::load(Transaction* t, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			t->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			t->setComment(rows.getColumn(rowId, TransactionTableSync::TABLE_COL_COMMENT));
			t->setStartDateTime(DateTime::FromSQLTimestamp(rows.getColumn(rowId, TransactionTableSync::TABLE_COL_START_DATE_TIME)));
			t->setEndDateTime(DateTime::FromSQLTimestamp(rows.getColumn(rowId, TransactionTableSync::TABLE_COL_END_DATE_TIME)));
			t->setDocumentId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionTableSync::TABLE_COL_DOCUMENT_ID)));
			t->setLeftUserId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionTableSync::TABLE_COL_LEFT_USER_ID)));
			t->setName(rows.getColumn(rowId, TransactionTableSync::TABLE_COL_NAME));
			t->setPlaceid(Conversion::ToLongLong(rows.getColumn(rowId, TransactionTableSync::TABLE_COL_PLACE_ID)));
		}

		template<> void SQLiteTableSyncTemplate<Transaction>::save(const db::SQLiteQueueThreadExec* sqlite, Transaction* t)
		{
			try
			{
				if (t->getKey() > 0)
				{
					/// @todo UPDATE implementation
				}
				else
				{
					t->setKey(getId(1,1));	/// @todo Handle grid
					stringstream query;
					query << "INSERT INTO " << TABLE_NAME << " VALUES("
						<< Conversion::ToString(t->getKey())
						<< "," << Conversion::ToSQLiteString(t->getName())
						<< "," << Conversion::ToString(t->getDocumentId())
						<< "," << t->getStartDateTime().toSQLiteString()
						<< "," << t->getEndDateTime().toSQLiteString()
						<< "," << Conversion::ToString(t->getLeftUserId())
						<< "," << Conversion::ToString(t->getPlaceId())
						<< "," << Conversion::ToSQLiteString(t->getComment())
						<< ")";
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
		const std::string TransactionTableSync::TABLE_COL_ID = "id";
		const std::string TransactionTableSync::TABLE_COL_NAME = "name";
		const std::string TransactionTableSync::TABLE_COL_DOCUMENT_ID = "document_id";
		const std::string TransactionTableSync::TABLE_COL_START_DATE_TIME = "start_date_time";
		const std::string TransactionTableSync::TABLE_COL_END_DATE_TIME = "end_date_time";
		const std::string TransactionTableSync::TABLE_COL_LEFT_USER_ID = "left_user_id";
		const std::string TransactionTableSync::TABLE_COL_PLACE_ID = "place_id";
		const std::string TransactionTableSync::TABLE_COL_COMMENT = "comment";

		TransactionTableSync::TransactionTableSync()
			: SQLiteTableSyncTemplate<Transaction>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_DOCUMENT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_START_DATE_TIME, "TIMESTAMP", true);
			addTableColumn(TABLE_COL_END_DATE_TIME, "TIMESTAMP", true);
			addTableColumn(TABLE_COL_LEFT_USER_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_PLACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_COMMENT, "TEXT", true);
		}

		void TransactionTableSync::rowsAdded( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionTableSync::rowsUpdated( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}
	}
}
