
/** VinciBikeTableSync class implementation.
	@file VinciBikeTableSync.cpp

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

#include "01_util/Exception.h"

#include "02_db/DBModule.h"

#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace vinci;
	using namespace util;
	using namespace accounts;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciBike>::TABLE_NAME = "t032_vinci_bike";
		template<> const int SQLiteTableSyncTemplate<VinciBike>::TABLE_ID = 32;
		template<> const bool SQLiteTableSyncTemplate<VinciBike>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciBike>::load(VinciBike* bike, const db::SQLiteResultSPtr& rows, int rowId)
		{
			bike->setKey(rows->getLongLong (TABLE_COL_ID));
			bike->setNumber(rows->get ( VinciBikeTableSync::TABLE_COL_NUMBER));
			bike->setMarkedNumber(rows->get ( VinciBikeTableSync::TABLE_COL_MARKED_NUMBER));
		}

		template<> void SQLiteTableSyncTemplate<VinciBike>::save(VinciBike* bike)
		{
			if (bike->getKey() <= 0)
				bike->setKey(getId()); /// @todo Handle grid number

			stringstream query;
			query << "REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(bike->getKey())
				<< "," << Conversion::ToSQLiteString(bike->getNumber())
				<< "," << Conversion::ToSQLiteString(bike->getMarkedNumber())
				<< ")";

			DBModule::GetSQLite()->execUpdate(query.str());
		}

	}

	namespace vinci
	{
		const std::string VinciBikeTableSync::TABLE_COL_NUMBER = "number";
		const std::string VinciBikeTableSync::TABLE_COL_MARKED_NUMBER = "marked_number";

		VinciBikeTableSync::VinciBikeTableSync()
			: db::SQLiteTableSyncTemplate<VinciBike>(true, true, TRIGGERS_ENABLED_CLAUSE, true)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NUMBER, "TEXT", true);
			addTableColumn(TABLE_COL_MARKED_NUMBER, "TEXT", true);

			addTableIndex(TABLE_COL_NUMBER);
			addTableIndex(TABLE_COL_MARKED_NUMBER);
		}

		
		/** Action to do on user creation.
		No action because the users are not permanently loaded in ram.
		*/
		void VinciBikeTableSync::rowsAdded (db::SQLite* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{}

		/** Action to do on user creation.
		Updates the users objects in the opened sessions.
		*/
		void VinciBikeTableSync::rowsUpdated (db::SQLite* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResultSPtr& rows)
		{}

		/** Action to do on user deletion.
		Closes the sessions of the deleted user.
		*/
		void VinciBikeTableSync::rowsRemoved (db::SQLite* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResultSPtr& rows)
		{}

		/** Example of support request :

		@code
SELECT *
	, (	SELECT 
			t031_transactions.id 
		FROM 
			t030_transaction_parts 
			INNER JOIN t031_transactions ON t031_transactions.id = t030_transaction_parts.transaction_id 
		WHERE
			t030_transaction_parts.traded_object=t032_vinci_bike.id
		ORDER BY
			t031_transactions.start_date_time DESC
		LIMIT 1
		) AS transaction_id
	, (	SELECT 
			t035_vinci_contract.id
		FROM
			t030_transaction_parts
			INNER JOIN t031_transactions ON t031_transactions.id = t030_transaction_parts.transaction_id
			INNER JOIN t035_vinci_contract ON t035_vinci_contract.user_id = t031_transactions.left_user_id
		WHERE
			t030_transaction_parts.traded_object=t032_vinci_bike.id
		ORDER BY
			t031_transactions.start_date_time DESC
		LIMIT 1
		) AS contract_id
FROM
	t032_vinci_bike
@endcode
		*/
		std::vector<shared_ptr<VinciBike> > VinciBikeTableSync::search(
			const std::string& id
			, const std::string& cadre 
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByNumber
			, bool orderByCadre
			, bool raisingOrder
			)
		{
			db::SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query 
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (!id.empty())
				query << " AND " << TABLE_COL_NUMBER << "='" << id << "' ";
			if (!cadre.empty())
				query << " AND " << TABLE_COL_MARKED_NUMBER << " LIKE '%" << cadre << "%' ";
			if (orderByNumber)
				query << " ORDER BY CAST (" << TABLE_COL_NUMBER << " AS INTEGER)" << (raisingOrder ? " ASC" : " DESC");
			if (orderByCadre)
				query << " ORDER BY " << TABLE_COL_MARKED_NUMBER << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (number > 0)
				query << " OFFSET " << first;

			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciBike> > bikes;
			for (int i=0; i<result.getNbRows(); ++i)
			{
				shared_ptr<VinciBike> bike(new VinciBike);
				try
				{
					load(bike.get(), result, i);
					VinciBike::Complements c;
					stringstream query2;
					query2 << "SELECT tt." << TABLE_COL_ID << " AS transaction_id"
						<< " FROM " 
						<< TransactionPartTableSync::TABLE_NAME << " AS tp" 
						<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS tt ON tt." << TABLE_COL_ID << "=tp." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID
						<< " WHERE tp." << TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID << "=" << Conversion::ToString(bike->getKey())
						<< " ORDER BY tt." << TransactionTableSync::TABLE_COL_START_DATE_TIME << " DESC"
						<< " LIMIT 1";
					SQLiteResultSPtr rows2 = sqlite->execQuery(query2.str());

					if (result2.getNbRows() > 0)
						c.lastTransaction = TransactionTableSync::get(Conversion::ToLongLong(result2.getColumn(0, "transaction_id")));

					stringstream query3;
					query3
						<< "SELECT cc." << TABLE_COL_ID  << " AS contract_id"
						<< " FROM " 
						<< TransactionPartTableSync::TABLE_NAME << " AS cp" 
						<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS ct ON ct." << TABLE_COL_ID << "=cp." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID
						<< " INNER JOIN " << VinciContractTableSync::TABLE_NAME << " AS cc ON cc." << VinciContractTableSync::COL_USER_ID << "=ct." << TransactionTableSync::TABLE_COL_LEFT_USER_ID
						<< " WHERE cp." << TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID << "=" << Conversion::ToString(bike->getKey())
						<< " ORDER BY ct." << TransactionTableSync::TABLE_COL_START_DATE_TIME << " DESC"
						<< " LIMIT 1";
					SQLiteResultSPtr rows3 = sqlite->execQuery(query3.str());

					if (result3.getNbRows() > 0)
						c.lastContract = VinciContractTableSync::get(Conversion::ToLongLong(result3.getColumn(0, "contract_id")));
					bike->setComplements(c);
					bikes.push_back(bike);
				}
				catch (Exception e)
				{

				}
			}
			return bikes;
		}

		boost::shared_ptr<VinciContract> VinciBikeTableSync::getRentContract( boost::shared_ptr<const VinciBike> bike )
		{
			stringstream query;
			query
				<< "SELECT c.*"
				<< " FROM "
					<< TransactionPartTableSync::TABLE_NAME << " AS p"
					<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID
					<< " INNER JOIN " << VinciContractTableSync::TABLE_NAME << " AS c ON c." << VinciContractTableSync::COL_USER_ID << "=t." << TransactionTableSync::TABLE_COL_LEFT_USER_ID
				<< " WHERE "
					<< "p." << TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID << "=" << Conversion::ToString(bike->getKey())
					<< " AND t." << TransactionTableSync::TABLE_COL_END_DATE_TIME << " IS NULL"
				<< " ORDER BY "
					<< "t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << " DESC"
				<< " LIMIT 1 ";
			SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
			if (!result.getNbRows())
				return shared_ptr<VinciContract>();
			shared_ptr<VinciContract> contract(new VinciContract);
			VinciContractTableSync::load(contract.get(), result, 0);
			return contract;
		}

		boost::shared_ptr<accounts::TransactionPart> VinciBikeTableSync::getRentTransactionPart( boost::shared_ptr<const VinciBike> bike )
		{
			stringstream query;
			query
				<< "SELECT p.*"
				<< " FROM "
					<< TransactionPartTableSync::TABLE_NAME << " AS p"
					<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID
				<< " WHERE "
					<< "p." << TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID << "=" << Conversion::ToString(bike->getKey())
					<< " AND t." << TransactionTableSync::TABLE_COL_END_DATE_TIME << " IS NULL"
				<< " ORDER BY "
					<< "t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << " DESC"
				<< " LIMIT 1 ";
			SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
			if (!result.getNbRows())
				return shared_ptr<TransactionPart>();
			shared_ptr<TransactionPart> tp(new TransactionPart);
			TransactionPartTableSync::load(tp.get(), result, 0);
			return tp;
		}
	}
}

