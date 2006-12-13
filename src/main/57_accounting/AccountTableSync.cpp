
#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/RegistryKeyException.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteException.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace util;

	namespace db
	{
		const std::string SQLiteTableSyncTemplate<Account>::TABLE_NAME = "t028_account";
		const int SQLiteTableSyncTemplate<Account>::TABLE_ID = 28;
		const bool SQLiteTableSyncTemplate<Account>::HAS_AUTO_INCREMENT = true;

		void SQLiteTableSyncTemplate<Account>::load(Account* account, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			try
			{
				account->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
				account->setLeftClassNumber(rows.getColumn(rowId, AccountTableSync::TABLE_COL_LEFT_CLASS_NUMBER));
				account->setLeftCurrency(AccountingModule::getCurrencies().get(Conversion::ToLongLong(rows.getColumn(rowId, AccountTableSync::TABLE_COL_LEFT_CURRENCY_ID))));
				account->setLeftNumber(rows.getColumn(rowId, AccountTableSync::TABLE_COL_LEFT_NUMBER));
				account->setLeftUserId(Conversion::ToLongLong(rows.getColumn(rowId, AccountTableSync::TABLE_COL_LEFT_USER_ID)));
				account->setRightClassNumber(rows.getColumn(rowId, AccountTableSync::TABLE_COL_RIGHT_CLASS_NUMBER));
				account->setRightCurrency(AccountingModule::getCurrencies().get(Conversion::ToLongLong(rows.getColumn(rowId, AccountTableSync::TABLE_COL_RIGHT_CURRENCY_ID))));
				account->setRightNumber(rows.getColumn(rowId, AccountTableSync::TABLE_COL_RIGHT_NUMBER));
				account->setRightUserId(Conversion::ToLongLong(rows.getColumn(rowId, AccountTableSync::TABLE_COL_RIGHT_USER_ID)));
				account->setName(rows.getColumn(rowId, AccountTableSync::TABLE_COL_NAME));
			}
			catch (Currency::RegistryKeyException& e)
			{
			}
		}

		void SQLiteTableSyncTemplate<Account>::save(const db::SQLiteThreadExec* sqlite, Account* account)
		{
			stringstream query;
			if (account->getKey() == 0)
			{
				account->setKey(getId(1,1));
				query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(account->getKey())
					<< "," << Conversion::ToString(account->getLeftUserId())
					<< "," << Conversion::ToSQLiteString(account->getName())
					<< "," << Conversion::ToSQLiteString(account->getLeftNumber())
					<< "," << Conversion::ToSQLiteString(account->getLeftClassNumber())
					<< "," << Conversion::ToString(account->getLeftCurrency()->getKey())
					<< "," << Conversion::ToString(account->getRightUserId())
					<< "," << Conversion::ToSQLiteString(account->getRightNumber())
					<< "," << Conversion::ToSQLiteString(account->getRightClassNumber())
					<< "," << Conversion::ToString(account->getRightCurrency()->getKey())
					<< ")";
			}
			else
			{
				// UPDATE
			}
			sqlite->execUpdate(query.str());
		}
	}

	namespace accounts
	{
		const std::string AccountTableSync::TABLE_COL_NAME = "name";
		const std::string AccountTableSync::TABLE_COL_LEFT_USER_ID = "left_user_id";
		const std::string AccountTableSync::TABLE_COL_LEFT_NUMBER = "left_number";
		const std::string AccountTableSync::TABLE_COL_LEFT_CLASS_NUMBER = "left_class_number";
		const std::string AccountTableSync::TABLE_COL_LEFT_CURRENCY_ID = "left_currency_id";
		const std::string AccountTableSync::TABLE_COL_RIGHT_USER_ID = "right_user_id";
		const std::string AccountTableSync::TABLE_COL_RIGHT_NUMBER = "right_number";
		const std::string AccountTableSync::TABLE_COL_RIGHT_CLASS_NUMBER = "right_class_number";
		const std::string AccountTableSync::TABLE_COL_RIGHT_CURRENCY_ID = "right_currency_id";



		void AccountTableSync::rowsAdded( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void AccountTableSync::rowsUpdated( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void AccountTableSync::rowsRemoved( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		AccountTableSync::AccountTableSync()
			: SQLiteTableSyncTemplate<Account>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_LEFT_USER_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_LEFT_NUMBER, "TEXT", true);
			addTableColumn(TABLE_COL_LEFT_CLASS_NUMBER, "TEXT", true);
			addTableColumn(TABLE_COL_LEFT_CURRENCY_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RIGHT_USER_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RIGHT_NUMBER, "TEXT", true);
			addTableColumn(TABLE_COL_RIGHT_CLASS_NUMBER, "TEXT", true);
			addTableColumn(TABLE_COL_RIGHT_CURRENCY_ID, "INTEGER", true);
		}

		std::vector<Account*> AccountTableSync::searchAccounts( const db::SQLiteThreadExec* sqlite , uid rightUserId
			, const std::string& rightClassNumber, uid leftUserId, const std::string& leftClassNumber, const std::string name
			, int first /*= 0*/, int number /*= 0*/ )
		{
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_RIGHT_USER_ID << "=" << Conversion::ToString(rightUserId)
				<< " AND " << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				<< " AND (" << TABLE_COL_LEFT_USER_ID << "=" << Conversion::ToString(leftUserId) << " OR " << TABLE_COL_LEFT_USER_ID << "=0 OR " << TABLE_COL_LEFT_USER_ID << "=\"\")"
				<< " AND " << TABLE_COL_LEFT_CLASS_NUMBER << " LIKE '%" << Conversion::ToSQLiteString(leftClassNumber, false) << "%'"
				<< " AND " << TABLE_COL_RIGHT_CLASS_NUMBER << " LIKE '%" << Conversion::ToSQLiteString(rightClassNumber, false) << "%'"
			;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<Account*> accounts;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					Account* account = new Account;
					load(account, result, i);
					accounts.push_back(account);
				}
				return accounts;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}