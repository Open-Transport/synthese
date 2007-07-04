
/** AccountTableSync class implementation.
	@file AccountTableSync.cpp

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

#include "01_util/Conversion.h"
#include "01_util/RegistryKeyException.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace util;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Account>::TABLE_NAME = "t028_account";
		template<> const int SQLiteTableSyncTemplate<Account>::TABLE_ID = 28;
		template<> const bool SQLiteTableSyncTemplate<Account>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Account>::load(Account* account, const db::SQLiteResult& rows, int rowId/*=0*/ )
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

		template<> void SQLiteTableSyncTemplate<Account>::save(Account* account)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (account->getKey() == 0)
			{
				account->setKey(getId());
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



		void AccountTableSync::rowsAdded( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{

		}

		void AccountTableSync::rowsUpdated( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void AccountTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		AccountTableSync::AccountTableSync()
			: SQLiteTableSyncTemplate<Account>(true, true, TRIGGERS_ENABLED_CLAUSE, true)
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

		std::vector<shared_ptr<Account> > AccountTableSync::search(uid rightUserId
			, const std::string& rightClassNumber, uid leftUserId, const std::string& leftClassNumber, const std::string name
			, int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
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
				vector<shared_ptr<Account> > accounts;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<Account> account(new Account);
					load(account.get(), result, i);
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
