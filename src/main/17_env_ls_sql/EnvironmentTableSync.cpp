#include "EnvironmentTableSync.h"

#include "01_util/Conversion.h"

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




EnvironmentTableSync::EnvironmentTableSync (synthese::env::Environment::Registry& environments)
: synthese::db::SQLiteTableSync (ENVIRONMENTS_TABLE_NAME)
  , _environments (environments)
{
    addTableColumn (TABLE_COL_ID, "INTEGER");
    addTableColumn (ENVIRONMENTS_TABLE_COL_LINKTABLE, "VARCHAR(50)");
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
	int envId = Conversion::ToInt (rows.getColumn (i, TABLE_COL_ID)); // TODO UID
	synthese::env::Environment* newEnv = new synthese::env::Environment (envId);
	_environments.add (newEnv);
	
	// Add a synchronizer on the new environment link table
	EnvironmentLinkTableSync* linkSync = 
	    new EnvironmentLinkTableSync (rows.getColumn (i, ENVIRONMENTS_TABLE_COL_LINKTABLE),
					  *newEnv);

	sync->addTableSynchronizer (linkSync);

	// Calls the init sequence on the new synchronizer
	linkSync->firstSync (sqlite, sync);
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
	int envId = Conversion::ToInt (rows.getColumn (i, TABLE_COL_ID)); // TODO UID
	std::string envTable = rows.getColumn (i, ENVIRONMENTS_TABLE_COL_LINKTABLE);
	std::string linkTableName = rows.getColumn (i, ENVIRONMENTS_TABLE_COL_LINKTABLE);
	// Drop the environment link table
	sqlite->execQuery ("DROP TABLE " + linkTableName);
	
	// Remove the environment
	_environments.remove (envId);
	
    }
}
	
	






}

}
