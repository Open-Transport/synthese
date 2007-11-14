
/** VinciRateTableSync class implementation.
	@file VinciRateTableSync.cpp

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

#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace vinci;
	using namespace db;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,VinciRateTableSync>::FACTORY_KEY("71.02 Vinci Rate");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciRateTableSync,VinciRate>::TABLE_NAME = "t033_vinci_rates";
		template<> const int SQLiteTableSyncTemplate<VinciRateTableSync,VinciRate>::TABLE_ID = 33;
		template<> const bool SQLiteTableSyncTemplate<VinciRateTableSync,VinciRate>::HAS_AUTO_INCREMENT = true;
		
		template<> void SQLiteTableSyncTemplate<VinciRateTableSync,VinciRate>::load(VinciRate* vr, const SQLiteResultSPtr& rows)
		{
			vr->setKey(rows->getLongLong (TABLE_COL_ID));
			vr->setName(rows->getText ( VinciRateTableSync::TABLE_COL_NAME));
			vr->setValidityDuration(rows->getDouble ( VinciRateTableSync::TABLE_COL_VALIDITY_DURATION));
			vr->setStartFinancialPrice(rows->getDouble ( VinciRateTableSync::TABLE_COL_START_FINANCIAL_PRICE));
			vr->setStartTicketsPrice(rows->getInt ( VinciRateTableSync::TABLE_COL_START_TICKETS_PRICE));
			vr->setEndFinancialPrice(rows->getDouble ( VinciRateTableSync::TABLE_COL_END_FINANCIAL_PRICE));
			vr->setEndTicketsPrice(rows->getInt ( VinciRateTableSync::TABLE_COL_END_TICKETS_PRICE));
			vr->setFirstPenalty(rows->getDouble ( VinciRateTableSync::TABLE_COL_FIRST_PENALTY));
			vr->setFirstPenaltyValidityDuration(rows->getDouble ( VinciRateTableSync::TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION));
			vr->setRecurringPenalty(rows->getDouble ( VinciRateTableSync::TABLE_COL_RECURRING_PENALTY));
			vr->setRecurringPenaltyPeriod(rows->getInt ( VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_PERIOD));
			vr->setRecurringPenaltyCancelsFirst(rows->getBool ( VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_CANCELS_FIRST));
		}

		template<> void SQLiteTableSyncTemplate<VinciRateTableSync,VinciRate>::_link(VinciRate* vr, const SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteTableSyncTemplate<VinciRateTableSync,VinciRate>::_unlink(VinciRate* vr)
		{

		}

		template<> void SQLiteTableSyncTemplate<VinciRateTableSync,VinciRate>::save(VinciRate* vr)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (vr->getKey() <= 0)
				vr->setKey(getId());
			query << "REPLACE INTO " << TABLE_NAME << " VALUES("
				<< vr->getKey()
				<< "," << Conversion::ToSQLiteString(vr->getName())
				<< "," << vr->getValidityDuration()
				<< "," << vr->getStartFinancialPrice()
				<< "," << vr->getStartTicketsPrice()
				<< "," << vr->getEndFinancialPrice()
				<< "," << vr->getEndTicketsPrice()
				<< "," << vr->getFirstPenalty()
				<< "," << vr->getFirstPenaltyValidityDuration()
				<< "," << vr->getRecurringPenalty()
				<< "," << vr->getRecurringPenaltyPeriod()
				<< "," << Conversion::ToString(vr->getRecurringPenaltyCancelsFirst())
				<< ")";
			sqlite->execUpdate(query.str());
		}
	}

	namespace vinci
	{
		const std::string VinciRateTableSync::TABLE_COL_NAME = "name";
		const std::string VinciRateTableSync::TABLE_COL_VALIDITY_DURATION = "validity_duration";
		const std::string VinciRateTableSync::TABLE_COL_START_FINANCIAL_PRICE = "start_financial_price";
		const std::string VinciRateTableSync::TABLE_COL_START_TICKETS_PRICE = "start_tickets_price";
		const std::string VinciRateTableSync::TABLE_COL_END_FINANCIAL_PRICE = "end_financial_price";
		const std::string VinciRateTableSync::TABLE_COL_END_TICKETS_PRICE = "end_tickets_price";
		const std::string VinciRateTableSync::TABLE_COL_FIRST_PENALTY = "first_penalty";
		const std::string VinciRateTableSync::TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION = "first_penalty_duration";
		const std::string VinciRateTableSync::TABLE_COL_RECURRING_PENALTY = "recurring_penalty";
		const std::string VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_PERIOD = "recurring_penalty_duration";
		const std::string VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_CANCELS_FIRST = "recurring_penalty_cancels_first";

		VinciRateTableSync::VinciRateTableSync()
			: SQLiteNoSyncTableSyncTemplate<VinciRateTableSync,VinciRate>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_VALIDITY_DURATION, "INTEGER", true);
			addTableColumn(TABLE_COL_START_FINANCIAL_PRICE, "REAL", true);
			addTableColumn(TABLE_COL_START_TICKETS_PRICE, "INT", true);
			addTableColumn(TABLE_COL_END_FINANCIAL_PRICE, "REAL", true);
			addTableColumn(TABLE_COL_END_TICKETS_PRICE, "INT", true);
			addTableColumn(TABLE_COL_FIRST_PENALTY, "REAL", true);
			addTableColumn(TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION, "INT", true);
			addTableColumn(TABLE_COL_RECURRING_PENALTY, "REAL", true);
			addTableColumn(TABLE_COL_RECURRING_PENALTY_PERIOD, "REAL", true);
			addTableColumn(TABLE_COL_RECURRING_PENALTY_CANCELS_FIRST, "BOOLEAN", true);
		}



		std::vector<shared_ptr<VinciRate> > VinciRateTableSync::search(
				 int first, int number)
		{
			db::SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT * FROM " << TABLE_NAME
				<< " ORDER BY " << TABLE_COL_NAME
				;

			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciRate> > rates;
			while(rows->next())
			{
				shared_ptr<VinciRate> rate(new VinciRate);
				try
				{
					load(rate.get (), rows);
					rates.push_back(rate);
				}
				catch (Exception e)
				{					
				}
			}
			return rates;
		}
	}
}
