
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

#include "04_time/Date.h"

#include "57_accounting/Account.h"

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
			map<Date, RentReportResult> m;
			Account* account = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
			if (account == NULL)
				return m;
			stringstream query;
			query
				<< "SELECT "
					<< "strftime('%Y-%m-%d', " << TransactionTableSync::TABLE_COL_START_DATE_TIME << ") AS day"
					<< ",COUNT(t." << TABLE_COL_ID << ") AS number"
				<< " FROM "
					<< TransactionTableSync::TABLE_NAME << " t"
					<< " INNER JOIN " << TransactionPartTableSync::TABLE_NAME << " p ON p." << TransactionPartTableSync::TABLE_COL_TRANSACTION_ID << "=t." << TABLE_COL_ID
				<< " WHERE "
				<< "p." << TransactionPartTableSync::TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
				<< " GROUP BY "
				<< "strftime('%Y-%m-%d', " << TransactionTableSync::TABLE_COL_START_DATE_TIME << ")"
				<< ",t." << TABLE_COL_ID;

			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				SQLiteResult result = sqlite->execQuery(query.str());
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					RentReportResult r;
					r.starts = result->getColumn(i, "number");
					m.insert(make_pair(Date::FromSQLString(result->getColumn(i, "day")), r));
				}
				return m;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
	
		}
	}
}
