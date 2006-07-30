#include "ServerConfigTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteThreadExec.h"
#include "04_time/DateTime.h"
#include "15_env/Environment.h"



#include "ServerConfig.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::db::SQLiteSync;
using synthese::db::SQLiteThreadExec;
using synthese::env::Environment;
using synthese::time::DateTime;

namespace synthese
{
namespace server
{



ServerConfigTableSync::ServerConfigTableSync (ServerConfig& config)
: synthese::db::SQLiteTableSync (CONFIG_TABLE_NAME, true, true)
  , _config (config)
{
    addTableColumn (CONFIG_TABLE_COL_PARAMNAME, "TEXT", false);
    addTableColumn (CONFIG_TABLE_COL_PARAMVALUE, "TIMESTAMP", true);
}



ServerConfigTableSync::~ServerConfigTableSync ()
{

}

    


void 
ServerConfigTableSync::rowsAdded (const SQLiteThreadExec* sqlite, 
				  SQLiteSync* sync,
				  const SQLiteResult& rows)
{
    for (int i=0; i<rows.getNbRows (); ++i)
    {
    
	std::string paramName = rows.getColumn (i, CONFIG_TABLE_COL_PARAMNAME);
	std::string paramValue = rows.getColumn (i, CONFIG_TABLE_COL_PARAMVALUE);
	
	if (paramName == CONFIG_TABLE_COL_PARAMVALUE_PORT)
	{
	    _config.setPort (Conversion::ToInt (paramValue));
	} 
	else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_NBTHREADS)
	{
	    _config.setNbThreads (Conversion::ToInt (paramValue));
	}
	else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_DATADIR)
	{
	    _config.setDataDir (boost::filesystem::path (paramValue));
	}
	else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_TEMPDIR)
	{
	    _config.setTempDir (boost::filesystem::path (paramValue));
	}
	else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPDIR)
	{
	    _config.setHttpTempDir (boost::filesystem::path (paramValue));
	}
	else if (paramName == CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPURL)
	{
	    _config.setHttpTempUrl (paramValue);
	}
    }
	
	
}


void 
ServerConfigTableSync::rowsUpdated (const SQLiteThreadExec* sqlite, 
				    SQLiteSync* sync,
				    const SQLiteResult& rows)
{
    rowsAdded (sqlite, sync, rows);
}



void 
ServerConfigTableSync::rowsRemoved (const SQLiteThreadExec* sqlite, 
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
