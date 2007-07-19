
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

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciRate>::TABLE_NAME = "t033_vinci_rates";
		template<> const int SQLiteTableSyncTemplate<VinciRate>::TABLE_ID = 33;
		template<> const bool SQLiteTableSyncTemplate<VinciRate>::HAS_AUTO_INCREMENT = true;
		
		template<> void SQLiteTableSyncTemplate<VinciRate>::load(VinciRate* vr, const SQLiteResult& rows, int rowId)
		{
			vr->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			vr->_name = rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_NAME);
			vr->_validityDuration = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_VALIDITY_DURATION));
			vr->_startFinancialPrice = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_START_FINANCIAL_PRICE));
			vr->_startTicketsPrice = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_START_TICKETS_PRICE));
			vr->_endFinancialPrice = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_END_FINANCIAL_PRICE));
			vr->_endTicketsPrice = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_END_TICKETS_PRICE));
			vr->_firstPenalty = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_FIRST_PENALTY));
			vr->_firstPenaltyValidityDuration = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION));
			vr->_recurringPenalty = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_RECURRING_PENALTY));
			vr->_recurringPenaltyPeriod = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_PERIOD));
			vr->_recurringPenaltyCancelsFirst = Conversion::ToBool(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_CANCELS_FIRST));
		}

		template<> void SQLiteTableSyncTemplate<VinciRate>::save(VinciRate* vr)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (vr->getKey() > 0)
			{	// UPDATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciRateTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(vr->_name)
					<< "," << VinciRateTableSync::TABLE_COL_VALIDITY_DURATION << "=" << vr->_validityDuration
					<< "," << VinciRateTableSync::TABLE_COL_START_FINANCIAL_PRICE << "=" << vr->_startFinancialPrice
					<< "," << VinciRateTableSync::TABLE_COL_START_TICKETS_PRICE << "=" << vr->_startTicketsPrice
					<< "," << VinciRateTableSync::TABLE_COL_END_FINANCIAL_PRICE << "=" << vr->_endFinancialPrice
					<< "," << VinciRateTableSync::TABLE_COL_END_TICKETS_PRICE << "=" << vr->_endTicketsPrice
					<< "," << VinciRateTableSync::TABLE_COL_FIRST_PENALTY << "=" << vr->_firstPenalty
					<< "," << VinciRateTableSync::TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION << "=" << vr->_firstPenaltyValidityDuration
					<< "," << VinciRateTableSync::TABLE_COL_RECURRING_PENALTY << "=" << vr->_recurringPenalty
					<< "," << VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_PERIOD << "=" << vr->_recurringPenaltyPeriod
					<< "," << VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_CANCELS_FIRST << "=" << Conversion::ToString(vr->_recurringPenaltyCancelsFirst)
					<< " WHERE " << TABLE_COL_ID << "=" << vr->getKey();
			}
			else
			{	// INSERT
				vr->setKey(getId(0,0));	/// @todo Handle grid name
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					<< vr->getKey()
					<< "," << Conversion::ToSQLiteString(vr->_name)
					<< "," << vr->_validityDuration
					<< "," << vr->_startFinancialPrice
					<< "," << vr->_startTicketsPrice
					<< "," << vr->_endFinancialPrice
					<< "," << vr->_endTicketsPrice
					<< "," << vr->_firstPenalty
					<< "," << vr->_firstPenaltyValidityDuration
					<< "," << vr->_recurringPenalty
					<< "," << vr->_recurringPenaltyPeriod
					<< "," << Conversion::ToString(vr->_recurringPenaltyCancelsFirst)
					<< ")";
			}
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
			: SQLiteTableSyncTemplate<VinciRate>(true, true, TRIGGERS_ENABLED_CLAUSE, true)
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

		void VinciRateTableSync::rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows, bool isFirstSync)
		{

		}

		
		void VinciRateTableSync::rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{

		}

		void VinciRateTableSync::rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{

		}

		std::vector<shared_ptr<VinciRate> > VinciRateTableSync::search(
				 int first, int number)
		{
			db::SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT * FROM " << TABLE_NAME
				<< " ORDER BY " << TABLE_COL_NAME
				;

			SQLiteResult result = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciRate> > rates;
			for (int i=0; i<result.getNbRows(); ++i)
			{
				shared_ptr<VinciRate> rate(new VinciRate);
				try
				{
					load(rate.get (), result, i);
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
