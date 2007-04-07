
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
			Account* account = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
			if (account == NULL)
				return m;

			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();

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

				SQLiteResult result = sqlite->execQuery(query.str());
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					RentReportResult r;
					r.starts = Conversion::ToInt(result.getColumn(i, COL_NUMBER));
					r.ends = 0;
					m.insert(make_pair(Date::FromSQLDate(result.getColumn(i, COL_DAY)), r));
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

				result = sqlite->execQuery(endQuery.str());
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					Date date = Date::FromSQLDate(result.getColumn(i, COL_DAY));
					map<Date, RentReportResult>::iterator it = m.find(date);
					if (it == m.end())
					{
						RentReportResult r;
						r.starts = 0;
						r.ends = Conversion::ToInt(result.getColumn(i, COL_NUMBER));
						m.insert(make_pair(date, r));
					}
					else
					{
						it->second.ends = Conversion::ToInt(result.getColumn(i, COL_NUMBER));
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
			Account* account = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
			if (account == NULL)
				return m;

			static const string COL_NUMBER("number");
			static const string COL_RATE("rate");

			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();

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

			SQLiteResult result = sqlite->execQuery(query.str());
			for (int i = 0; i < result.getNbRows(); ++i)
			{
				RentReportResult r;
				r.starts = Conversion::ToInt(result.getColumn(i, COL_NUMBER));
				r.ends = 0;
				m.insert(make_pair(Conversion::ToLongLong(result.getColumn(i, COL_RATE)), r));
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

			result = sqlite->execQuery(endQuery.str());
			for (int i = 0; i < result.getNbRows(); ++i)
			{
				uid rateId = Conversion::ToLongLong(result.getColumn(i, COL_RATE));
				map<uid, RentReportResult>::iterator it2 = m.find(rateId);
				if (it2 == m.end())
				{
					RentReportResult r;
					r.starts = 0;
					r.ends = Conversion::ToInt(result.getColumn(i, COL_NUMBER));;
					m.insert(make_pair(rateId, r));
				}
				else
				{
					it2->second.ends = Conversion::ToInt(result.getColumn(i, COL_NUMBER));
				}
			}

			return m;
		}
	}
}
