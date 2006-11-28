
#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteException.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Currency.h"
#include "57_accounting/CurrencyTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace accounts;

	namespace db
	{
		const std::string SQLiteTableSyncTemplate<Currency>::TABLE_NAME = "t029_currencies";
		const int SQLiteTableSyncTemplate<Currency>::TABLE_ID = 29;
		const bool SQLiteTableSyncTemplate<Currency>::HAS_AUTO_INCREMENT = true;
	}

	namespace accounts
	{
		const std::string CurrencyTableSync::TABLE_COL_ID = "id";
		const std::string CurrencyTableSync::TABLE_COL_NAME = "name";
		const std::string CurrencyTableSync::TABLE_COL_SYMBOL = "symbol";


		CurrencyTableSync::CurrencyTableSync()
			: SQLiteTableSyncTemplate<Currency>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_SYMBOL, "TEXT", true);
		}

		void CurrencyTableSync::rowsAdded( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				Currency* currency = new Currency;
				loadCurrency(currency, rows, i);
				AccountingModule::getCurrencies().add(currency);
			}
		}

		void CurrencyTableSync::rowsUpdated( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void CurrencyTableSync::rowsRemoved( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		std::vector<Currency*> CurrencyTableSync::searchCurrencies( const db::SQLiteThreadExec* sqlite , const std::string& name, const std::string& symbol , int first /*= 0*/, int number /*= 0*/ )
		{
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				<< " AND " << TABLE_COL_SYMBOL << " LIKE '%" << Conversion::ToSQLiteString(symbol, false) << "%'";
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<Currency*> currencies;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					Currency* currency = new Currency;
					loadCurrency(currency, result, i);
					currencies.push_back(currency);
				}
				return currencies;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}

		}

		void CurrencyTableSync::loadCurrency( Currency* currency, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			currency->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			currency->setName(rows.getColumn(rowId, TABLE_COL_NAME));
			currency->setSymbol(rows.getColumn(rowId, TABLE_COL_SYMBOL));
		}
	}
}