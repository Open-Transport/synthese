
#include <sstream>

#include "71_vinci_bike_rental/VinciSiteTableSync.h"

using namespace std;

namespace synthese
{
	
	namespace db
	{
		const std::string SQLiteTableSyncTemplate<VinciSite>::TABLE_NAME = "t034_vinci_site";
		const int SQLiteTableSyncTemplate<VinciSite>::TABLE_ID = 34;
		const bool SQLiteTableSyncTemplate<VinciSite>::HAS_AUTO_INCREMENT = true;
	}

	namespace vinci
	{
		const std::string VinciSiteTableSync::TABLE_COL_ID = "id";
		const std::string VinciSiteTableSync::TABLE_COL_NAME = "name";
		
		VinciSiteTableSync::VinciSiteTableSync()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
		}

		SQLiteTableSyncTemplate<VinciSite>::load(VinciSite* vs, const SQLiteResult& rows, int rowId)
		{
			vs->setId(Conversion::ToLongLong(rows.getColumn(rowId, VinciSiteTableSync::TABLE_COL_ID));
			vs->_name = rows.getColumn(rowId, VinciSiteTableSync::TABLE_COL_NAME);
		}

		SQLiteTableSyncTemplate<VinciSite>::save(SQLiteThreadExec* sqlite, VinciSite* vs)
		{
			stringstream query;
			if (vs->getId() != 0)
			{	// UPDATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciSiteTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(vs->_name)
					<< " WHERE " << VinciSiteTableSync::TABLE_COL_ID << "=" vs->getid();
			}
			else
			{	// INSERT
				vs->setId(getId());
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					vs->getId()
					<< "," << Conversion::ToSQLiteString(vs->_name)
					<< ")";
			}
			sqlite->execUpdate(query);
		}
	}
}