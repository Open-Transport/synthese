
#include <sqlite/sqlite3.h>

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "04_time/DateTime.h"

#include "15_env/Environment.h"

#include "30_server/ServerConfig.h"
#include "30_server/ServerModule.h"
#include "30_server/ServerConfigTableSync.h"

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace time;

	namespace server
	{

		ServerConfigTableSync::ServerConfigTableSync ()
		: synthese::db::SQLiteTableSync (CONFIG_TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (CONFIG_TABLE_COL_PARAMNAME, "TEXT", false);
			addTableColumn (CONFIG_TABLE_COL_PARAMVALUE, "TIMESTAMP", true);
		}



		ServerConfigTableSync::~ServerConfigTableSync ()
		{

		}

		    


		void 
		ServerConfigTableSync::rowsAdded (const SQLiteQueueThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows (); ++i)
			{
		    
			std::string paramName = rows.getColumn (i, CONFIG_TABLE_COL_PARAMNAME);
			std::string paramValue = rows.getColumn (i, CONFIG_TABLE_COL_PARAMVALUE);
			
			if (paramName == CONFIG_TABLE_COL_PARAMVALUE_PORT)
			{
				ServerModule::getConfig().setPort (Conversion::ToInt (paramValue));
			} 
			else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_NBTHREADS)
			{
				ServerModule::getConfig().setNbThreads (Conversion::ToInt (paramValue));
			}
			else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_LOGLEVEL)
			{
				ServerModule::getConfig().setLogLevel (Conversion::ToInt (paramValue));
			}
			else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_DATADIR)
			{
				ServerModule::getConfig().setDataDir (boost::filesystem::path (paramValue, boost::filesystem::native));
			}
			else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_TEMPDIR)
			{
				ServerModule::getConfig().setTempDir (boost::filesystem::path (paramValue, boost::filesystem::native));
			}
			else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPDIR)
			{
				ServerModule::getConfig().setHttpTempDir (boost::filesystem::path (paramValue, boost::filesystem::native));
			}
			else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPURL)
			{
				ServerModule::getConfig().setHttpTempUrl (paramValue);
			}
			}
			
			
		}


		void 
		ServerConfigTableSync::rowsUpdated (const SQLiteQueueThreadExec* sqlite, 
							SQLiteSync* sync,
							const SQLiteResult& rows)
		{
			rowsAdded (sqlite, sync, rows);
		}



		void 
		ServerConfigTableSync::rowsRemoved (const SQLiteQueueThreadExec* sqlite, 
							SQLiteSync* sync,
							const SQLiteResult& rows)
		{

		}





		/*
		void 
		ServerConfigTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex)
		{
			std::string paramName = rows.getColumn (rowIndex, CONFIG_TABLE_COL_PARAMNAME);
			std::string paramValue = rows.getColumn (rowIndex, CONFIG_TABLE_COL_PARAMVALUE);

			if (paramName == "")
			{
			
			}

		}



		void 
		ServerConfigTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex)
		{
		    
		}



		void 
		ServerConfigTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex)
		{

		}
		*/

	}
}

