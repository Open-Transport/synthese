
#include <sqlite/sqlite3.h>


#include "01_util/Conversion.h"

#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/EnvModule.h"
#include "15_env/EnvironmentSyncException.h"
#include "15_env/EnvironmentLinkTableSync.h"
#include "15_env/ComponentTableSync.h"

using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;

namespace env
{



EnvironmentLinkTableSync::EnvironmentLinkTableSync ()
: synthese::db::SQLiteTableSync (ENVIRONMENT_LINKS_TABLE_NAME, true, true, db::TRIGGERS_ENABLED_CLAUSE)
{	{
		addTableColumn (TABLE_COL_ID, "INTEGER", false);
		addTableColumn (ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID, "INTEGER", false);
		addTableColumn (ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID, "INTEGER", false);
	}
	// The preceding block is used by doxygen, please do not remove

}



EnvironmentLinkTableSync::~EnvironmentLinkTableSync ()
{

}


void 
EnvironmentLinkTableSync::beforeFirstSync (const synthese::db::SQLiteQueueThreadExec* sqlite, 
				     synthese::db::SQLiteSync* sync)
{
    std::map<std::string, shared_ptr<SQLiteTableSync> > tableSynchronizers = sync->getTableSynchronizers ();
    for (std::map<std::string, shared_ptr<SQLiteTableSync> >::const_iterator it = tableSynchronizers.begin ();
	 it != tableSynchronizers.end (); ++it)
    {
	
	const SQLiteTableSync* synchronizer = it->second.get();
	std::string tableName = synchronizer->getTableName ();
	
        // Parse table id from table name
	int tableId (synchronizer->getTableId ());
	_componentTableNames.insert (std::make_pair (tableId, tableName));
    }
}





    
void 
EnvironmentLinkTableSync::rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
				     synthese::db::SQLiteSync* sync,
				     const synthese::db::SQLiteResult& rows, bool isFirstSync)
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
	    shared_ptr<ComponentTableSync> componentTableSync = dynamic_pointer_cast<ComponentTableSync, SQLiteTableSync>
		(sync->getTableSynchronizer (componentTableName));
		componentTableSync->doAdd (existingRow, 0, *EnvModule::getEnvironments().getUpdateable(envId));
	}
    }
}



void 
EnvironmentLinkTableSync::rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
			     synthese::db::SQLiteSync* sync,
			     const synthese::db::SQLiteResult& rows)
{
    // Not supported right now...
    throw synthese::db::SQLiteException ("Updating an environment link is not supported right now.");
}



void 
EnvironmentLinkTableSync::rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
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
	/// @todo Check if necessary
	if (existingRow.getNbRows () == 1) 
	{
	    shared_ptr<ComponentTableSync> componentTableSync = dynamic_pointer_cast<ComponentTableSync, SQLiteTableSync>
		(sync->getTableSynchronizer (componentTableName));
		componentTableSync->doRemove (existingRow, 0, *EnvModule::getEnvironments().getUpdateable (envId).get());
	}
	_cache.erase (id);
    }

}
	
	






}

}

