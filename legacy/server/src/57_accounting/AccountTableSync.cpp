
/** AccountTableSync class implementation.
	@file AccountTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "Account.h"
#include "AccountTableSync.h"
#include "Currency.h"
#include "CurrencyTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,AccountTableSync>::FACTORY_KEY("57.10 Account");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<AccountTableSync>::TABLE.NAME = "t028_account";
		template<> const int DBTableSyncTemplate<AccountTableSync>::TABLE.ID = 28;
		template<> const bool DBTableSyncTemplate<AccountTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void DBDirectTableSyncTemplate<AccountTableSync,Account>::load(Account* account, const db::DBResultSPtr& rows )
		{
		    account->setKey(rows->getLongLong (TABLE_COL_ID));

			account->setLeftClassNumber(rows->getText ( AccountTableSync::TABLE_COL_LEFT_CLASS_NUMBER));
		    account->setLeftNumber(rows->getText ( AccountTableSync::TABLE_COL_LEFT_NUMBER));
		    account->setLeftUserId(rows->getLongLong ( AccountTableSync::TABLE_COL_LEFT_USER_ID));
		    account->setRightClassNumber(rows->getText ( AccountTableSync::TABLE_COL_RIGHT_CLASS_NUMBER));

		    account->setRightNumber(rows->getText ( AccountTableSync::TABLE_COL_RIGHT_NUMBER));
		    account->setRightUserId(rows->getLongLong ( AccountTableSync::TABLE_COL_RIGHT_USER_ID));
		    account->setName(rows->getText ( AccountTableSync::TABLE_COL_NAME));

			account->setLocked(rows->getBool(AccountTableSync::COL_LOCKED));
			account->setStockAccountId(rows->getLongLong(AccountTableSync::COL_STOCK_ACCOUNT_ID));
			account->setUnitPrice(rows->getDouble(AccountTableSync::COL_UNIT_PRICE));
		}

		template<> void DBDirectTableSyncTemplate<AccountTableSync,Account>::_link(Account* account, const db::DBResultSPtr& rows, GetSource temporary)
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

		template<> void DBDirectTableSyncTemplate<AccountTableSync,Account>::_unlink(Account* account)
		{
			account->setLeftCurrency(NULL);
			account->setRightCurrency(NULL);
		}

		template<> void DBDirectTableSyncTemplate<AccountTableSync,Account>::Save(
			Account* account,
			optional<DBTransaction&> transaction
		){
			// TODO: use ReplaceQuery instead of building the SQL.
			DB* db = DBModule::GetDB();
			stringstream query;
			if (account->getKey() <= 0)
				account->setKey(getId());
			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(account->getKey())
				<< "," << Conversion::ToDBString(account->getName())
				<< "," << Conversion::ToString(account->getLeftUserId())
				<< "," << Conversion::ToDBString(account->getLeftNumber())
				<< "," << Conversion::ToDBString(account->getLeftClassNumber())
				<< "," << Conversion::ToString(account->getLeftCurrency()->getKey())
				<< "," << Conversion::ToString(account->getRightUserId())
				<< "," << Conversion::ToDBString(account->getRightNumber())
				<< "," << Conversion::ToDBString(account->getRightClassNumber())
				<< "," << Conversion::ToString(account->getRightCurrency()->getKey())
				<< "," << Conversion::ToString(account->getLocked())
				<< "," << Conversion::ToString(account->getStockAccountId())
				<< "," << Conversion::ToString(account->getUnitPrice())
				<< ")";
			db->execUpdate(query.str(), transaction);
		}
	}

	namespace accounts
	{
		const string AccountTableSync::TABLE_COL_NAME = "name";
		const string AccountTableSync::TABLE_COL_LEFT_USER_ID = "left_user_id";
		const string AccountTableSync::TABLE_COL_LEFT_NUMBER = "left_number";
		const string AccountTableSync::TABLE_COL_LEFT_CLASS_NUMBER = "left_class_number";
		const string AccountTableSync::TABLE_COL_LEFT_CURRENCY_ID = "left_currency_id";
		const string AccountTableSync::TABLE_COL_RIGHT_USER_ID = "right_user_id";
		const string AccountTableSync::TABLE_COL_RIGHT_NUMBER = "right_number";
		const string AccountTableSync::TABLE_COL_RIGHT_CLASS_NUMBER = "right_class_number";
		const string AccountTableSync::TABLE_COL_RIGHT_CURRENCY_ID = "right_currency_id";
		const string AccountTableSync::COL_LOCKED("locked");
		const string AccountTableSync::COL_STOCK_ACCOUNT_ID("stock_account_id");
		const string AccountTableSync::COL_UNIT_PRICE("unit_price");



		AccountTableSync::AccountTableSync()
			: DBNoSyncTableSyncTemplate<AccountTableSync,Account>()
		{
			addTableColumn(TABLE_COL_ID, "SQL_INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "SQL_TEXT", true);
			addTableColumn(TABLE_COL_LEFT_USER_ID, "SQL_INTEGER", true);
			addTableColumn(TABLE_COL_LEFT_NUMBER, "SQL_TEXT", true);
			addTableColumn(TABLE_COL_LEFT_CLASS_NUMBER, "SQL_TEXT", true);
			addTableColumn(TABLE_COL_LEFT_CURRENCY_ID, "SQL_INTEGER", true);
			addTableColumn(TABLE_COL_RIGHT_USER_ID, "SQL_INTEGER", true);
			addTableColumn(TABLE_COL_RIGHT_NUMBER, "SQL_TEXT", true);
			addTableColumn(TABLE_COL_RIGHT_CLASS_NUMBER, "SQL_TEXT", true);
			addTableColumn(TABLE_COL_RIGHT_CURRENCY_ID, "SQL_INTEGER", true);
			addTableColumn(COL_LOCKED, "SQL_INTEGER", true);
			addTableColumn(COL_STOCK_ACCOUNT_ID, "SQL_INTEGER", true);
			addTableColumn(COL_UNIT_PRICE, "SQL_REAL", true);
		}


		std::vector<shared_ptr<Account> > AccountTableSync::search(
			uid rightUserId
			, const std::string& rightClassNumber
			, uid leftUserId
			, const std::string& leftClassNumber
			, const std::string name
			, bool orderByName
			, bool raisingOrder
			, int first /*= 0*/
			, int number /*= 0*/
		){
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " << TABLE_COL_RIGHT_USER_ID << "=" << Conversion::ToString(rightUserId)
				<< " AND " << TABLE_COL_NAME << " LIKE '%" << Conversion::ToDBString(name, false) << "%'"
				<< " AND (" << TABLE_COL_LEFT_USER_ID << "=" << Conversion::ToString(leftUserId) << " OR " << TABLE_COL_LEFT_USER_ID << "=0 OR " << TABLE_COL_LEFT_USER_ID << "=\"\")"
				<< " AND " << TABLE_COL_LEFT_CLASS_NUMBER << " LIKE '" << Conversion::ToDBString(leftClassNumber, false) << "'"
				<< " AND " << TABLE_COL_RIGHT_CLASS_NUMBER << " LIKE '" << Conversion::ToDBString(rightClassNumber, false) << "'"
			;
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				DBResultSPtr rows = db->execQuery(query.str());
				vector<shared_ptr<Account> > accounts;
				while (rows->next ())
				{
					shared_ptr<Account> account(new Account);
					load(account.get(), rows);
					accounts.push_back(account);
				}
				return accounts;
			}
			catch(DBException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		std::string AccountTableSync::GetNextCode( std::string basisCode )
		{
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< " SELECT MAX(" << TABLE_COL_RIGHT_CLASS_NUMBER << ") AS nu"
				<< " FROM " << TABLE.NAME
				<< " WHERE " << TABLE_COL_RIGHT_CLASS_NUMBER << " LIKE '" << basisCode << "%'";
			try
			{
				DBResultSPtr rows = db->execQuery(query.str());
				int nextCode(1);
				if (rows->next() && !rows->getText("nu").empty())
				{
					string number(rows->getText("nu"));
					number = number.substr(basisCode.size());
					nextCode = Conversion::ToInt(number) + 1;
				}
				return basisCode + Conversion::ToFixedSizeString(nextCode, 5);
			}
			catch(DBException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
