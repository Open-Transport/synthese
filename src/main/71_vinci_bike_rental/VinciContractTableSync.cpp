
#include <sstream>

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"

using namespace std;

namespace synthese
{
	
	namespace db
	{
		const std::string SQLiteTableSyncTemplate<VinciContract>::TABLE_NAME = "t035_vinci_contract";
		const int SQLiteTableSyncTemplate<VinciContract>::TABLE_ID = 35;
		const bool SQLiteTableSyncTemplate<VinciContract>::HAS_AUTO_INCREMENT = true;
	}

	namespace vinci
	{
		const std::string VinciContractTableSync::TABLE_COL_ID = "id";
		const std::string VinciContractTableSync::TABLE_COL_USER_ID = "user_id";
		
		VinciContractTableSync::VinciContractTableSync()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_USER_ID, "INTEGER", true);
		}

		SQLiteTableSyncTemplate<VinciContract>::load(VinciContract* vc, const SQLiteResult& rows, int rowId)
		{
			vc->setId(Conversion::ToLongLong(rows.getColumn(rowId, VinciContractTableSync::TABLE_COL_ID));
			vc->_userId = rows.getColumn(rowId, VinciContractTableSync::TABLE_COL_USER_ID);
		}

		SQLiteTableSyncTemplate<VinciContract>::save(SQLiteThreadExec* sqlite, VinciContract* vc)
		{
			stringstream query;
			if (vc->getId() != 0)
			{	// UPDATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciContractTableSync::TABLE_COL_USER_ID << "=" << Conversion::ToSQLiteString(vc->_userId)
					<< " WHERE " << VinciContractTableSync::TABLE_COL_ID << "=" vc->getid();
			}
			else
			{	// INSERT
				vc->setId(getId());
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					vc->getId()
					<< "," << Conversion::ToString(vc->_userId)
					<< ")";
			}
			sqlite->execUpdate(query);
		}
	}
}