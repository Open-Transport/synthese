
#include <sstream>

#include "71_vinci_bike_rental/VinciRateTableSync.h"

using namespace std;

namespace synthese
{
	
	namespace db
	{
		const std::string SQLiteTableSyncTemplate<VinciRate>::TABLE_NAME = "t033_transaction_parts";
		const int SQLiteTableSyncTemplate<VinciRate>::TABLE_ID = 33;
		const bool SQLiteTableSyncTemplate<VinciRate>::HAS_AUTO_INCREMENT = true;
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
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_VALIDITY_DURATION, "INTEGER", true);
			addTableColumn(TABLE_COL_START_FINANCIAL_PRICE, "REAL", true);
			addTableColumn(TABLE_COL_START_TICKETS_PRICE, "INT", true);
			addTableColumn(TABLE_COL_END_FINANCIAL_PRICE, "REAL", true);
			addTableColumn(TABLE_COL_END_TICKETS_PRICE, "INT", true);
			addTableColumn(TABLE_COL_END_FIRST_PENALTY, "REAL", true);
			addTableColumn(TABLE_COL_END_FIRST_PENALTY_VALIDITY_DURATION, "INT", true);
			addTableColumn(TABLE_COL_END_RECURRING_PENALTY, "REAL", true);
			addTableColumn(TABLE_COL_END_RECURRING_PENALTY_PERIOD, "REAL", true);
		}

		SQLiteTableSyncTemplate<VinciRate>::load(VinciRate* vr, const SQLiteResult& rows, int rowId)
		{
			vr->setId(Conversion::ToLongLong(rows.getColumn(rowId, VinciRateTableSync::TABLE_COL_ID));
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

		SQLiteTableSyncTemplate<VinciRate>::save(SQLiteThreadExec* sqlite, VinciRate* vr)
		{
			stringstream query;
			if (vr->getId() != 0)
			{	// UPDATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciRateTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(vr->_name)
					<< "," << VinciRateTableSync::TABLE_COL_VALIDITY_DURATION << "=" << vr->_validityDuration
					<< "," << VinciRateTableSync::TABLE_COL_START_FINANCIAL_PRICE << "=" << vr->_startFinancialPrice
					<< "," << VinciRateTablesync::TABLE_COL_START_TICKETS_PRICE << "=" << vr->_startTicketPrice
					<< "," << VinciRateTableSync::TABLE_COL_END_FINANCIAL_PRICE << "=" << vr->_endFinancialPrice
					<< "," << VinciRateTablesync::TABLE_COL_END_TICKETS_PRICE << "=" << vr->_endTicketPrice
					<< "," << VinciRateTableSync::TABLE_COL_FIRST_PENALTY << "=" << vr->_firstPenalty;
					<< "," << VinciRateTableSync::TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION << "=" << vr->_firstPenaltyValidityDuration;
					<< "," << VinciRateTableSync::TABLE_COL_RECURRING_PENALTY << "=" << vr->_recurringPenalty;
					<< "," << VinciRateTableSync::TABLE_COL_RECURRING_PENALTY_PERIOD << "=" << vr->_recurringPenaltyPeriod;
					<< " WHERE " << VinciRateTableSync::TABLE_COL_ID << "=" << vr->getId();
			}
			else
			{	// INSERT
				vr->setId(vr->getId());
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					vr->getId()
					<< "," << Conversion::ToSQLiteString(vr->_name)
					<< "," << vr->_validityDuration
					<< "," << vr->_startFinancialPrice
					<< "," << vr->_startTicketPrice
					<< "," << vr->_endFinancialPrice
					<< "," << vr->_endTicketPrice
					<< "," << vr->_firstPenalty;
					<< "," << vr->_firstPenaltyValidityDuration;
					<< "," << vr->_recurringPenalty;
					<< "," << vr->_recurringPenaltyPeriod;
					<< ")";
			}
			sqlite->execUpdate(query);
		}
	}
}