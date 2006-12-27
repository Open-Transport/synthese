
#include <sstream>

#include "30_server/ServerModule.h"

#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"

using namespace std;

namespace synthese
{
	using namespace vinci;
	using namespace db;
	using namespace server;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciRate>::TABLE_NAME = "t033_vinci_rates";
		template<> const int SQLiteTableSyncTemplate<VinciRate>::TABLE_ID = 33;
		template<> const bool SQLiteTableSyncTemplate<VinciRate>::HAS_AUTO_INCREMENT = true;
		
		template<> void SQLiteTableSyncTemplate<VinciRate>::load(VinciRate* vr, const SQLiteResult& rows, int rowId)
		{
			vr->setKey(Conversion::ToLongLong(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_ID)));
			vr->_name = rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_NAME);
			vr->_validityDuration = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_VALIDITY_DURATION));
			vr->_startFinancialPrice = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_START_FINANCIAL_PRICE));
			vr->_startTicketsPrice = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_START_TICKETS_PRICE));
			vr->_endFinancialPrice = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_END_FINANCIAL_PRICE));
			vr->_endTicketsPrice = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_END_TICKETS_PRICE));
			vr->_firstPenalty = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_FIRST_PENALTY));
			vr->_firstPenaltyValidityDuration = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION));
			vr->_recurringPenalty = Conversion::ToDouble(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_RECURRING_PENALTY));
			vr->_recurringPenaltyPeriod = Conversion::ToInt(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_PERIOD));
		}

		template<> void SQLiteTableSyncTemplate<VinciRate>::save(const SQLiteQueueThreadExec* sqlite, VinciRate* vr)
		{
			stringstream query;
			if (vr->getKey() != 0)
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
					<< " WHERE " << VinciRateTableSync::TABLE_COL_ID << "=" << vr->getKey();
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
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}
	}

	namespace vinci
	{
		const std::string VinciRateTableSync::TABLE_COL_ID = "id";
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

		VinciRateTableSync::VinciRateTableSync()
			: SQLiteTableSyncTemplate<VinciRate>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
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
		}

		void VinciRateTableSync::rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{

		}

		
		void VinciRateTableSync::rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{

		}

		void VinciRateTableSync::rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{

		}

		std::vector<VinciRate*> VinciRateTableSync::searchVinciRates(
				 int first, int number)
		{
			const db::SQLiteQueueThreadExec* sqlite = ServerModule::getSQLiteThread();
			stringstream query;
			query
				<< " SELECT * FROM " << TABLE_NAME
				<< " ORDER BY " << TABLE_COL_NAME
				;

			SQLiteResult result = sqlite->execQuery(query.str());
			vector<VinciRate*> rates;
			for (int i=0; i<result.getNbRows(); ++i)
			{
				VinciRate* rate = new VinciRate;
				try
				{
					load(rate, result, i);
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
