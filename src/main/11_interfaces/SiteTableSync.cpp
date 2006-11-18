
#include "SiteTableSync.h"
#include "02_db/SQLiteResult.h"
#include "01_util/Conversion.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace db
	{
		const std::string SiteTableSync::TABLE_NAME = "t025_sites";
		const std::string SiteTableSync::TABLE_COL_ID = "id";
		const std::string SiteTableSync::TABLE_COL_NAME = "name";
		const std::string SiteTableSync::TABLE_COL_INTERFACE_ID = "interface_id";
		const std::string SiteTableSync::TABLE_COL_ENVIRONMENT_ID = "env_id";
		const std::string SiteTableSync::TABLE_COL_START_DATE = "start_date";
		const std::string SiteTableSync::TABLE_COL_END_DATE = "end_date";
		const std::string SiteTableSync::TABLE_COL_ONLINE_BOOKING = "online_booking";
		const std::string SiteTableSync::TABLE_COL_USE_OLD_DATA = "use_old_data";
		const std::string SiteTableSync::TABLE_COL_CLIENT_URL = "client_url";


		SiteTableSync::SiteTableSync(synthese::interfaces::Site::Registry& sites
			, const std::string& triggerOverrideClause 
			, const synthese::interfaces::Interface::Registry& interfaces
			, const synthese::env::Environment::Registry& environments)
			: SQLiteTableSync ( TABLE_NAME, true, true, triggerOverrideClause )
			, _sites(sites)
			, _environments(environments)
			, _interfaces(interfaces)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_INTERFACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_ENVIRONMENT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_START_DATE, "DATE", true);
			addTableColumn(TABLE_COL_END_DATE, "DATE", true);
			addTableColumn(TABLE_COL_ONLINE_BOOKING, "INTEGER", true);
			addTableColumn(TABLE_COL_USE_OLD_DATA, "INTEGER", true);
			addTableColumn(TABLE_COL_CLIENT_URL, "TEXT", true);
		}


		void SiteTableSync::rowsUpdated( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}


		void SiteTableSync::rowsAdded( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				Site* site = new Site(Conversion::ToLongLong(rows.getColumn(i,TABLE_COL_ID)));
				_sites.add(site);

				

	
			}
		}


		void SiteTableSync::rowsRemoved( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}
	}
}


