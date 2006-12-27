
#include <sqlite/sqlite3.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"

#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/EnvModule.h"
#include "15_env/EnvironmentLinkTableSync.h"
#include "15_env/EnvironmentTableSync.h"


using synthese::util::Conversion;


namespace synthese
{
	using namespace db;

namespace env
{




EnvironmentTableSync::EnvironmentTableSync ()
: synthese::db::SQLiteTableSync (ENVIRONMENTS_TABLE_NAME, true, true, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (TABLE_COL_ID, "INTEGER");
}



EnvironmentTableSync::~EnvironmentTableSync ()
{

}

    
void 
EnvironmentTableSync::rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
				 synthese::db::SQLiteSync* sync,
				 const synthese::db::SQLiteResult& rows)
{
    for (int i=0; i<rows.getNbRows (); ++i)
    {
		uid envId = Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID));
		if (EnvModule::getEnvironments().contains (envId)) continue;
		synthese::env::Environment* newEnv = new synthese::env::Environment (envId);
		EnvModule::getEnvironments().add (newEnv);
    }
}



void 
EnvironmentTableSync::rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
			     synthese::db::SQLiteSync* sync,
			     const synthese::db::SQLiteResult& rows)
{
    // Not supported right now...
    throw synthese::db::SQLiteException ("Updating an environment row is not supported right now.");

}



void 
EnvironmentTableSync::rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
				 synthese::db::SQLiteSync* sync,
				 const synthese::db::SQLiteResult& rows)
{
    // Look in environment link tables for each row id
    for (int i=0; i<rows.getNbRows (); ++i)
    {
		std::string envId = rows.getColumn (i, TABLE_COL_ID);

		// Remove all environment links
		sqlite->execQuery ("DELETE FROM " + ENVIRONMENT_LINKS_TABLE_NAME + " WHERE " 
				+ ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + "=" + envId);
		
		// Remove the environment
		EnvModule::getEnvironments().remove (Conversion::ToLongLong (envId));
	
    }
}
	
	






}

}

