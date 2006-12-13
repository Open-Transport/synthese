
#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"

#include "04_time/Date.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfaceModule.h"
#include "15_env/Environment.h"
#include "15_env/EnvModule.h"

#include "30_server/ServerModule.h"
#include "30_server/SiteTableSync.h"

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace time;

	namespace server
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


		SiteTableSync::SiteTableSync()
			: db::SQLiteTableSync ( TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
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
				Site* site = NULL;
				try
				{
					site = new Site(Conversion::ToLongLong(rows.getColumn(i,TABLE_COL_ID)));
					site->setName(rows.getColumn(i,TABLE_COL_NAME));
					Interface* interf = InterfaceModule::getInterfaces().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_INTERFACE_ID)));
					site->setInterface(interf);
					Environment* env = EnvModule::getEnvironments().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ENVIRONMENT_ID)));
					site->setEnvironment(env);
					site->setStartDate(Date::FromSQLDate(rows.getColumn(i, TABLE_COL_START_DATE)));
					site->setEndDate(Date::FromSQLDate(rows.getColumn(i, TABLE_COL_END_DATE)));
					site->setOnlineBookingAllowed(Conversion::ToBool(rows.getColumn(i, TABLE_COL_ONLINE_BOOKING)));
					site->setPastSolutionsDisplayed(Conversion::ToBool(rows.getColumn(i, TABLE_COL_USE_OLD_DATA)));
				}
				catch (Interface::RegistryKeyException e)
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE_NAME +" table : Interface not found", e);
					delete site;
					continue;
				}
				catch (Environment::RegistryKeyException e)
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE_NAME +" table : Environment not found", e);
					delete site;
					continue;
				}

				ServerModule::getSites().add(site);
			}
		}


		void SiteTableSync::rowsRemoved( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}
	}
}



