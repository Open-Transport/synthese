#include "EnvironmentTableSync.h"

#include "01_util/Conversion.h"
#include "01_util/UId.h"

#include <sqlite/sqlite3.h>
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "EnvironmentLinkTableSync.h"


using synthese::util::Conversion;
using synthese::db::SQLiteResult;


namespace synthese
{
namespace envlssql
{




EnvironmentTableSync::EnvironmentTableSync (synthese::env::Environment::Registry& environments,
					    const std::string& triggerOverrideClause)
: synthese::db::SQLiteTableSync (ENVIRONMENTS_TABLE_NAME, true, true, triggerOverrideClause)
  , _environments (environments)
{
    addTableColumn (TABLE_COL_ID, "INTEGER");
}



EnvironmentTableSync::~EnvironmentTableSync ()
{

}

    
void 
EnvironmentTableSync::rowsAdded (const synthese::db::SQLiteThreadExec* sqlite, 
				 synthese::db::SQLiteSync* sync,
				 const synthese::db::SQLiteResult& rows)
{
    for (int i=0; i<rows.getNbRows (); ++i)
    {
	uid envId = Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID));
	synthese::env::Environment* newEnv = new synthese::env::Environment (envId);
	_environments.add (newEnv);
    }
}



void 
EnvironmentTableSync::rowsUpdated (const synthese::db::SQLiteThreadExec* sqlite, 
			     synthese::db::SQLiteSync* sync,
			     const synthese::db::SQLiteResult& rows)
{
    // Not supported right now...
    throw synthese::db::SQLiteException ("Updating an environment row is not supported right now.");

}



void 
EnvironmentTableSync::rowsRemoved (const synthese::db::SQLiteThreadExec* sqlite, 
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
	_environments.remove (Conversion::ToLongLong (envId));
	
    }
}
	
	






}

}
