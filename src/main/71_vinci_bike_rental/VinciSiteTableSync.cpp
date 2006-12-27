
#include <sstream>

#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace vinci;
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciSite>::TABLE_NAME = "t034_vinci_site";
		template<> const int SQLiteTableSyncTemplate<VinciSite>::TABLE_ID = 34;
		template<> const bool SQLiteTableSyncTemplate<VinciSite>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciSite>::load(VinciSite* vs, const SQLiteResult& rows, int rowId)
		{
			vs->setKey(Conversion::ToLongLong(rows.getColumn(rowId, VinciSiteTableSync::TABLE_COL_ID)));
			vs->_name = rows.getColumn(rowId, VinciSiteTableSync::TABLE_COL_NAME);
		}

		template<> void SQLiteTableSyncTemplate<VinciSite>::save(const SQLiteQueueThreadExec* sqlite, VinciSite* vs)
		{
			stringstream query;
			if (vs->getKey() != 0)
			{	// UPDATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciSiteTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(vs->_name)
					<< " WHERE " << VinciSiteTableSync::TABLE_COL_ID << "=" << vs->getKey();
			}
			else
			{	// INSERT
				vs->setKey(getId(0,0)); /// @todo Handle grid id
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					<< vs->getKey()
					<< "," << Conversion::ToSQLiteString(vs->_name)
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}
	}

	namespace vinci
	{
		const std::string VinciSiteTableSync::TABLE_COL_ID = "id";
		const std::string VinciSiteTableSync::TABLE_COL_NAME = "name";
		
		VinciSiteTableSync::VinciSiteTableSync()
			: SQLiteTableSyncTemplate<VinciSite>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
		}

		void VinciSiteTableSync::rowsAdded( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void VinciSiteTableSync::rowsUpdated( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void VinciSiteTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}
	}
}
