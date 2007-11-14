
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

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Currency.h"
#include "57_accounting/CurrencyTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,AccountTableSync>::FACTORY_KEY("57.10 Account");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<AccountTableSync,Account>::TABLE_NAME = "t028_account";
		template<> const int SQLiteTableSyncTemplate<AccountTableSync,Account>::TABLE_ID = 28;
		template<> const bool SQLiteTableSyncTemplate<AccountTableSync,Account>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<AccountTableSync,Account>::load(Account* account, const db::SQLiteResultSPtr& rows )
		{
			    account->setKey(rows->getLongLong (TABLE_COL_ID));
			
				account->setLeftClassNumber(rows->getText ( AccountTableSync::TABLE_COL_LEFT_CLASS_NUMBER));
			    account->setLeftNumber(rows->getText ( AccountTableSync::TABLE_COL_LEFT_NUMBER));
			    account->setLeftUserId(rows->getLongLong ( AccountTableSync::TABLE_COL_LEFT_USER_ID));
			    account->setRightClassNumber(rows->getText ( AccountTableSync::TABLE_COL_RIGHT_CLASS_NUMBER));

			    account->setRightNumber(rows->getText ( AccountTableSync::TABLE_COL_RIGHT_NUMBER));
			    account->setRightUserId(rows->getLongLong ( AccountTableSync::TABLE_COL_RIGHT_USER_ID));
			    account->setName(rows->getText ( AccountTableSync::TABLE_COL_NAME));
		}

		template<> void SQLiteTableSyncTemplate<AccountTableSync,Account>::_link(Account* account, const db::SQLiteResultSPtr& rows, GetSource temporary)
		{
			try
			{
				account->setLeftCurrency(CurrencyTableSync::Get(
						rows->getLongLong ( AccountTableSync::TABLE_COL_LEFT_CURRENCY_ID)
						, account
						, false
						, temporary
				));
				account->setRightCurrency(CurrencyTableSync::Get(
						rows->getLongLong ( AccountTableSync::TABLE_COL_RIGHT_CURRENCY_ID)
						, account
						, false
						, temporary
				));
			}
			catch (Currency::ObjectNotFoundException& e)
			{
			}
		}

		template<> void SQLiteTableSyncTemplate<AccountTableSync,Account>::_unlink(Account* account)
		{
			account->setLeftCurrency(NULL);
			account->setRightCurrency(NULL);
		}

		template<> void SQLiteTableSyncTemplate<AccountTableSync,Account>::save(Account* account)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (account->getKey() <= 0)
				account->setKey(getId());
			query
				<< "REPLACE INTO " << TABLE_NAME << " VALUES("
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



		AccountTableSync::AccountTableSync()
			: SQLiteNoSyncTableSyncTemplate<AccountTableSync,Account>()
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
			SQLite* sqlite = DBModule::GetSQLite();
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
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<Account> > accounts;
				while (rows->next ())
				{
					shared_ptr<Account> account(new Account);
					load(account.get(), rows);
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
