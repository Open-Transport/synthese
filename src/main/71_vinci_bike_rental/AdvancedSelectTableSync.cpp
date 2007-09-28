
/** AdvancedSelectTableSync class implementation.
	@file AdvancedSelectTableSync.cpp

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

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"

#include "04_time/Date.h"

#include "57_accounting/Account.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPartTableSync.h"

#include "71_vinci_bike_rental/AdvancedSelectTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace accounts;
	using namespace time;
	using namespace db;

	namespace vinci
	{


		std::map<time::Date, RentReportResult> getRentsPerDay( const time::Date& start, const time::Date& end )
		{
			static const string COL_NUMBER("number");
			static const string COL_DAY("day");

			map<Date, RentReportResult> m;
			shared_ptr<Account> account = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
			if (!account.get())
				return m;

			try
			{
				SQLite* sqlite = DBModule::GetSQLite();

				// Rent Starts
				stringstream query;
				query
					<< "SELECT "
						<< "strftime('%Y-%m-%d', " << TransactionTableSync::TABLE_COL_START_DATE_TIME << ") AS " << COL_DAY
						<< ",COUNT(t." << TABLE_COL_ID << ") AS " << COL_NUMBER
					<< " FROM "
						<< TransactionTableSync::TABLE_NAME << " t"
						<< " INNER JOIN " << TransactionPartTableSync::TABLE_NAME << " p ON p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID << "=t." << TABLE_COL_ID
					<< " WHERE "
						<< "p." << TransactionPartTableSync::TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
						<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << ">=" << start.toSQLString()
						<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << "<=" << end.toSQLString()
					<< " GROUP BY "
						<< "strftime('%Y-%m-%d', " << TransactionTableSync::TABLE_COL_START_DATE_TIME << ")"
					;

				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				while (rows->next ())
				{
					RentReportResult r;
					r.starts = rows->getInt ( COL_NUMBER);
					r.ends = 0;
					m.insert(make_pair(Date::FromSQLDate(rows->getText (COL_DAY)), r));
				}
	
				// Rent ends
				stringstream endQuery;
				endQuery
					<< "SELECT "
						<< "strftime('%Y-%m-%d', " << TransactionTableSync::TABLE_COL_END_DATE_TIME << ") AS " << COL_DAY
						<< ",COUNT(t." << TABLE_COL_ID << ") AS " << COL_NUMBER
					<< " FROM "
						<< TransactionTableSync::TABLE_NAME << " t"
						<< " INNER JOIN " << TransactionPartTableSync::TABLE_NAME << " p ON p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID << "=t." << TABLE_COL_ID
					<< " WHERE "
						<< "p." << TransactionPartTableSync::TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
						<< " AND t." << TransactionTableSync::TABLE_COL_END_DATE_TIME << ">=" << start.toSQLString()
						<< " AND t." << TransactionTableSync::TABLE_COL_END_DATE_TIME << "<=" << end.toSQLString()
					<< " GROUP BY "
						<< "strftime('%Y-%m-%d', " << TransactionTableSync::TABLE_COL_END_DATE_TIME << ")"
					;
				
				rows = sqlite->execQuery(endQuery.str());
				while (rows->next ())
				{
				    Date date = Date::FromSQLDate(rows->getText (COL_DAY));
				    map<Date, RentReportResult>::iterator it = m.find(date);
				    if (it == m.end())
				    {
					RentReportResult r;
					r.starts = 0;
					r.ends = rows->getInt (COL_NUMBER);
					m.insert(make_pair(date, r));
				    }
				    else
				    {
					it->second.ends = rows->getInt (COL_NUMBER);
				    }
				}

				return m;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
	
		}

		std::map<uid, RentReportResult> getRentsPerRate( const time::Date& start, const time::Date& end )
		{
			map<uid, RentReportResult> m;
			shared_ptr<Account> account = VinciBikeRentalModule::getAccount(
			    VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);

			if (!account.get())
			    return m;

			static const string COL_NUMBER("number");
			static const string COL_RATE("rate");

			SQLite* sqlite = DBModule::GetSQLite();

			// Rent Starts
			stringstream query;
			query
				<< "SELECT "
					<< "p." << TransactionPartTableSync::TABLE_COL_RATE_ID << " AS " << COL_RATE
					<< ",COUNT(t." << TABLE_COL_ID << ") AS " << COL_NUMBER
				<< " FROM "
					<< TransactionTableSync::TABLE_NAME << " t"
					<< " INNER JOIN " << TransactionPartTableSync::TABLE_NAME << " p ON p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID << "=t." << TABLE_COL_ID
				<< " WHERE "
					<< "p." << TransactionPartTableSync::TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << ">=" << start.toSQLString()
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << "<=" << end.toSQLString()
				<< " GROUP BY "
					<< "p." << TransactionPartTableSync::TABLE_COL_RATE_ID
				;

			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			while (rows->next ())
			{
				RentReportResult r;
				r.starts = rows->getInt (COL_NUMBER);
				r.ends = 0;
				m.insert(make_pair(rows->getLongLong (COL_RATE), r));
			}

			// Rent ends
			stringstream endQuery;
			endQuery
				<< "SELECT "
				<< "p." << TransactionPartTableSync::TABLE_COL_RATE_ID << " AS " << COL_RATE
					<< ",COUNT(t." << TABLE_COL_ID << ") AS " << COL_NUMBER
				<< " FROM "
					<< TransactionTableSync::TABLE_NAME << " t"
					<< " INNER JOIN " << TransactionPartTableSync::TABLE_NAME << " p ON p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID << "=t." << TABLE_COL_ID
				<< " WHERE "
					<< "p." << TransactionPartTableSync::TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
					<< " AND t." << TransactionTableSync::TABLE_COL_END_DATE_TIME << ">=" << start.toSQLString()
					<< " AND t." << TransactionTableSync::TABLE_COL_END_DATE_TIME << "<=" << end.toSQLString()
				<< " GROUP BY "
					<< "p." << TransactionPartTableSync::TABLE_COL_RATE_ID
				;

			rows = sqlite->execQuery (endQuery.str());
			while (rows->next ())
			{
			    uid rateId = rows->getLongLong (COL_RATE);
			    map<uid, RentReportResult>::iterator it2 = m.find(rateId);
				if (it2 == m.end())
				{
					RentReportResult r;
					r.starts = 0;
					r.ends = rows->getInt (COL_NUMBER);
					m.insert(make_pair(rateId, r));
				}
				else
				{
				    it2->second.ends = rows->getInt (COL_NUMBER);
				}
			}

			return m;
		}
	}
}
