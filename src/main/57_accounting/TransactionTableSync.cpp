
/** TransactionTableSync class implementation.
	@file TransactionTableSync.cpp

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

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPartTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace time;
	using namespace accounts;
	using namespace util;
	

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TransactionTableSync>::FACTORY_KEY("57.20 Transaction");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TransactionTableSync,Transaction>::TABLE_NAME = "t031_transactions";
		template<> const int SQLiteTableSyncTemplate<TransactionTableSync,Transaction>::TABLE_ID = 31;
		template<> const bool SQLiteTableSyncTemplate<TransactionTableSync,Transaction>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<TransactionTableSync,Transaction>::load(Transaction* t, const db::SQLiteResultSPtr& rows )
		{
		    t->setKey(rows->getLongLong (TABLE_COL_ID));
		    t->setComment(rows->getText ( TransactionTableSync::TABLE_COL_COMMENT));
		    t->setStartDateTime(DateTime::FromSQLTimestamp(rows->getText ( TransactionTableSync::TABLE_COL_START_DATE_TIME)));
		    t->setEndDateTime(DateTime::FromSQLTimestamp(rows->getText ( TransactionTableSync::TABLE_COL_END_DATE_TIME)));
		    t->setDocumentId(rows->getLongLong ( TransactionTableSync::TABLE_COL_DOCUMENT_ID));
		    t->setLeftUserId(rows->getLongLong ( TransactionTableSync::TABLE_COL_LEFT_USER_ID));
		    t->setName(rows->getText ( TransactionTableSync::TABLE_COL_NAME));
		    t->setPlaceid(rows->getLongLong ( TransactionTableSync::TABLE_COL_PLACE_ID));
		}



		template<> void SQLiteTableSyncTemplate<TransactionTableSync,Transaction>::_link(Transaction* t, const db::SQLiteResultSPtr& rows, GetSource temporary )
		{

		}


		template<> void SQLiteTableSyncTemplate<TransactionTableSync,Transaction>::_unlink(Transaction* t)
		{

		}


		template<> void SQLiteTableSyncTemplate<TransactionTableSync,Transaction>::save(Transaction* t)
		{
		    try
		    {
				SQLite* sqlite = DBModule::GetSQLite();
				stringstream query;
				if (t->getKey() <= 0)
					t->setKey(getId());
				query
					<< "REPLACE INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(t->getKey())
					<< "," << Conversion::ToSQLiteString(t->getName())
					<< "," << Conversion::ToString(t->getDocumentId())
					<< "," << t->getStartDateTime().toSQLString()
					<< "," << t->getEndDateTime().toSQLString()
					<< "," << Conversion::ToString(t->getLeftUserId())
					<< "," << Conversion::ToString(t->getPlaceId())
					<< "," << Conversion::ToSQLiteString(t->getComment())
					<< ")";
				sqlite->execUpdate(query.str());
		    }
		    catch (SQLiteException& e)
		    {
			
		    }
		}
	    
	}
    


	namespace accounts
	{
		const std::string TransactionTableSync::TABLE_COL_NAME = "name";
		const std::string TransactionTableSync::TABLE_COL_DOCUMENT_ID = "document_id";
		const std::string TransactionTableSync::TABLE_COL_START_DATE_TIME = "start_date_time";
		const std::string TransactionTableSync::TABLE_COL_END_DATE_TIME = "end_date_time";
		const std::string TransactionTableSync::TABLE_COL_LEFT_USER_ID = "left_user_id";
		const std::string TransactionTableSync::TABLE_COL_PLACE_ID = "place_id";
		const std::string TransactionTableSync::TABLE_COL_COMMENT = "comment";

		TransactionTableSync::TransactionTableSync()
			: SQLiteNoSyncTableSyncTemplate<TransactionTableSync,Transaction>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_DOCUMENT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_START_DATE_TIME, "TIMESTAMP", true);
			addTableColumn(TABLE_COL_END_DATE_TIME, "TIMESTAMP", true);
			addTableColumn(TABLE_COL_LEFT_USER_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_PLACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_COMMENT, "TEXT", true);

			addTableIndex(TABLE_COL_START_DATE_TIME);
			vector<string> v;
			v.push_back(TABLE_COL_LEFT_USER_ID);
			v.push_back(TABLE_COL_START_DATE_TIME);
			addTableIndex(v);
		}



		shared_ptr<TransactionPart> TransactionTableSync::getPart(shared_ptr<const Transaction> transaction, shared_ptr<const Account> account )
		{
			vector<shared_ptr<TransactionPart> > vtp = TransactionPartTableSync::search(transaction, account, 0, 1);
			return vtp.empty() ? shared_ptr<TransactionPart>() : vtp.front();
		}

	}
}
