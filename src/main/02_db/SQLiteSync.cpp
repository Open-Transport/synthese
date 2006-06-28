#include "SQLiteSync.h"

#include "01_util/Conversion.h"
#include "SQLiteThreadExec.h"
#include "SQLiteTableSync.h"
#include "SQLiteException.h"

#include <sqlite/sqlite3.h>

#include <assert.h>


using synthese::util::Conversion;


namespace synthese
{
namespace db
{





SQLiteSync::SQLiteSync (const std::string& idColumnName)
: _idColumnName (idColumnName)
{

}



SQLiteSync::~SQLiteSync ()
{
}

 

void 
SQLiteSync::addTableSynchronizer (SQLiteTableSync* synchronizer)
{
    boost::mutex::scoped_lock lock (_tableSynchronizersMutex);

    assert (synchronizer->getTableFormat ().empty () == false);
    _tableSynchronizers.insert (std::make_pair (synchronizer->getTableName (), synchronizer));
}



bool 
SQLiteSync::hasTableSynchronizer (const std::string& tableName) const
{
    boost::mutex::scoped_lock lock (_tableSynchronizersMutex);
    return _tableSynchronizers.find (tableName) != _tableSynchronizers.end ();
}


   
SQLiteTableSync* 
SQLiteSync::getTableSynchronizer (const std::string& tableName) const
{
    boost::mutex::scoped_lock lock (_tableSynchronizersMutex);
    if (hasTableSynchronizer (tableName) == false)
    {
	throw SQLiteException ("No synchronizer for table '" + tableName + "'");
    }
    return _tableSynchronizers.find (tableName)->second;
}


void 
SQLiteSync::registerCallback (const SQLiteThreadExec* emitter)
{
    boost::mutex::scoped_lock lock (_tableSynchronizersMutex);

    // Call the init sequence on all synchronizers.
    for (std::map<std::string, SQLiteTableSync*>::const_iterator it = _tableSynchronizers.begin ();
	 it != _tableSynchronizers.end (); ++it)
    {
	it->second->firstSync (emitter, this);
    }

}
 

   
void 
SQLiteSync::eventCallback (const SQLiteThreadExec* emitter,
			   const SQLiteEvent& event)
{
    boost::mutex::scoped_lock lock (_tableSynchronizersMutex);

    for (std::map<std::string, SQLiteTableSync* >::const_iterator it = _tableSynchronizers.begin ();
	 it != _tableSynchronizers.end (); ++it)
    {
	SQLiteTableSync* tableSync = it->second;
	if (tableSync->getTableName () != event.tbName) continue;
	
	if (event.opType == SQLITE_INSERT) 
	{
	    // Query for the modified row
	    SQLiteResult result = emitter->execQuery ("SELECT * FROM " + event.tbName + " WHERE " 
						      + _idColumnName + "=" + Conversion::ToString (event.rowId));
	    
	    tableSync->rowsAdded (emitter, this, result);
	}
	else if (event.opType == SQLITE_UPDATE) 
	{
	    // Query for the modified row
	    SQLiteResult result = emitter->execQuery ("SELECT * FROM " + event.tbName + " WHERE " 
						      + _idColumnName + "=" + Conversion::ToString (event.rowId));

	    tableSync->rowsUpdated (emitter, this, result);
	}
	else if (event.opType == SQLITE_DELETE) 
	{
	    // Query for the modified row
	    SQLiteResult result;
	    std::vector<std::string> columns;
	    std::vector<std::string> values;
	    columns.push_back (_idColumnName);
	    values.push_back (Conversion::ToString (event.rowId));
	    result.addRow (values, columns);

	    tableSync->rowsRemoved (emitter, this, result);
	}
    }
    
}















}

}
