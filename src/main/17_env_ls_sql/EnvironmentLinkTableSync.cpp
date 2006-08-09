#include "EnvironmentLinkTableSync.h"

#include "01_util/Conversion.h"
#include "EnvironmentSyncException.h"

#include <sqlite/sqlite3.h>
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "CityTableSync.h"



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;


namespace synthese
{
namespace envlssql
{



EnvironmentLinkTableSync::EnvironmentLinkTableSync (const synthese::db::SQLiteSync* sync,
						    Environment::Registry& environments,
						    const std::string& triggerOverrideClause)
: synthese::db::SQLiteTableSync (ENVIRONMENT_LINKS_TABLE_NAME, true, true, triggerOverrideClause)
, _environments (environments)
{	{
		addTableColumn (TABLE_COL_ID, "INTEGER", false);
		addTableColumn (ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID, "INTEGER", false);
		addTableColumn (ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID, "INTEGER", false);
	}
	// The preceding block is used by doxygen, please do not remove

    const std::map<std::string, SQLiteTableSync* >& tableSynchronizers = sync->getTableSynchronizers ();
    for (std::map<std::string, SQLiteTableSync* >::const_iterator it = tableSynchronizers.begin ();
	 it != tableSynchronizers.end (); ++it)
    {
	const SQLiteTableSync* synchronizer = it->second;
	std::string tableName = synchronizer->getTableName ();
	
        // Parse table id from table name
	int tableId (synchronizer->getTableId ());
	_componentTableNames.insert (std::make_pair (tableId, tableName));
    }
}



EnvironmentLinkTableSync::~EnvironmentLinkTableSync ()
{

}

    
void 
EnvironmentLinkTableSync::rowsAdded (const synthese::db::SQLiteThreadExec* sqlite, 
				     synthese::db::SQLiteSync* sync,
				     const synthese::db::SQLiteResult& rows)
{
    for (int i=0; i<rows.getNbRows (); ++i)
    {
	uid envId = Conversion::ToLongLong (rows.getColumn (i, ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID));

	std::string linkTargetStr (rows.getColumn (i, ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID));
	uid linkTargetId = Conversion::ToLongLong (linkTargetStr);
	int tableId = synthese::util::decodeTableId (linkTargetId);

	// Find the corresponding table name
	std::map<int, std::string>::const_iterator it = _componentTableNames.find (tableId);
	if (it == _componentTableNames.end ()) continue;

	// Update the cache
	_cache.insert (std::make_pair (rows.getColumn (i, TABLE_COL_ID), rows.getColumns (i)));

	// Check if the component has already been added in its corresponding table
	std::string componentTableName (it->second);
	SQLiteResult existingRow = sqlite->execQuery ("SELECT * FROM " + componentTableName + 
						      " WHERE " + TABLE_COL_ID + "=" + linkTargetStr);

	if (existingRow.getNbRows () == 1) 
	{
	    ComponentTableSync* componentTableSync = dynamic_cast<ComponentTableSync*>
		(sync->getTableSynchronizer (componentTableName));
	    componentTableSync->doAdd (existingRow, 0, *_environments.get (envId));
	}
    }
}



void 
EnvironmentLinkTableSync::rowsUpdated (const synthese::db::SQLiteThreadExec* sqlite, 
			     synthese::db::SQLiteSync* sync,
			     const synthese::db::SQLiteResult& rows)
{
    // Not supported right now...
    throw synthese::db::SQLiteException ("Updating an environment link is not supported right now.");
}



void 
EnvironmentLinkTableSync::rowsRemoved (const synthese::db::SQLiteThreadExec* sqlite, 
				       synthese::db::SQLiteSync* sync,
				       const synthese::db::SQLiteResult& rows)
{
    for (int i=0; i<rows.getNbRows (); ++i)
    {
	std::string id = rows.getColumn (i, TABLE_COL_ID);


	// Get back cached values for concerned rows
	std::vector<std::string> cachedValues = _cache.find (id)->second;
	uid envId = Conversion::ToLongLong (cachedValues.at (1));
	std::string linkTargetStr = cachedValues.at (2);

	uid linkTargetId = Conversion::ToLongLong (linkTargetStr);
	int tableId = synthese::util::decodeTableId (linkTargetId);

	std::map<int, std::string>::const_iterator it = _componentTableNames.find (tableId);
	if (it == _componentTableNames.end ()) continue;

	std::string componentTableName (it->second);

	SQLiteResult existingRow = sqlite->execQuery ("SELECT * FROM " + componentTableName + 
						      " WHERE " + TABLE_COL_ID + "=" + linkTargetStr);

	if (existingRow.getNbRows () == 1) 
	{
	    ComponentTableSync* componentTableSync = dynamic_cast<ComponentTableSync*>
		(sync->getTableSynchronizer (componentTableName));
	    componentTableSync->doRemove (existingRow, 0, *_environments.get (envId));
	}
	_cache.erase (id);
    }

}
	
	






}

}
