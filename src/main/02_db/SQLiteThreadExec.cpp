#include "SQLiteThreadExec.h"

#include "01_util/Conversion.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteResult.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>


#include "01_util/Log.h"


using synthese::util::Conversion;
using synthese::util::Log;
using synthese::db::SQLite;
using synthese::db::SQLiteEvent;
using synthese::db::SQLiteResult;



namespace synthese
{
namespace db
{

boost::mutex eventQueueMutex; 
    



void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId)
{
    // WARNING : the update hook is invoked only when working with the connection
    // created inside the body of this thread (initialize).
    SQLiteThreadExec* exec = (SQLiteThreadExec*) userData;

    SQLiteEvent event;
    event.opType = opType;
    event.dbName = dbName;
    event.tbName = tbName;
    event.rowId = rowId;

    exec->enqueueEvent (event);

}





SQLiteThreadExec::SQLiteThreadExec (const boost::filesystem::path& databaseFile)
: _databaseFile (databaseFile)
, _db (0)
, _hooksMutex (new boost::mutex ())
, _queueMutex (new boost::recursive_mutex ())
, _dbMutex (new boost::recursive_mutex ())

{
}



SQLiteThreadExec::~SQLiteThreadExec ()
{
}




void 
SQLiteThreadExec::registerUpdateHook (SQLiteUpdateHook* hook)
{
    boost::mutex::scoped_lock hooksLock (*_hooksMutex);

    // Lock db til register callback is executed (can use this db connection)
    boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);

    _hooks.push_back (hook);
    if (_db != 0)
    {
	// database handle has been initialized
	// call the register callback directly
	hook->registerCallback (this);
    }
}




void 
SQLiteThreadExec::postEvent (const SQLiteEvent& event) const
{
    boost::mutex::scoped_lock hooksLock (*_hooksMutex);

    for (std::vector<SQLiteUpdateHook*>::const_iterator it = _hooks.begin ();
	 it != _hooks.end (); ++it)
    {
	(*it)->eventCallback (this, event);
    }
    
}





void 
SQLiteThreadExec::enqueueEvent (const SQLiteEvent& event)
{
    boost::recursive_mutex::scoped_lock queueLock (*_queueMutex);
    _eventQueue.push_back (event);
}








void
SQLiteThreadExec::initialize()
{
    // The database cannot be updated til all the hooks have been initialized
    // through their registerCallback.
    boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);

    // No hook can be added til the thread has finished its initialization.
    boost::mutex::scoped_lock hooksLock (*_hooksMutex);

    // Open a persistent DB connection
    // It is crucial that the db connection is created inside the init proc
    // so that the connection is created in the caller thread.
    _db = SQLite::OpenConnection (_databaseFile);

    // Initialize each hooks
    std::for_each (_hooks.begin(), _hooks.end (), 
		   std::bind2nd (std::mem_fun (&SQLiteUpdateHook::registerCallback), this));

    // Install the update hook
    sqlite3_update_hook (_db, &sqliteUpdateHook, this);

}




void
SQLiteThreadExec::loop()
{
    // Lock the db to ensure that the handle is used by only one thread at
    // the same time!
    boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);
    boost::recursive_mutex::scoped_lock queueLock (*_queueMutex);
    while (_eventQueue.empty () == false) 
    {
	SQLiteEvent event = _eventQueue.front ();
	_eventQueue.pop_front ();
	postEvent (event);
    }    
}




void
SQLiteThreadExec::finalize()
{
    SQLite::CloseConnection (_db);
}




SQLiteResult 
SQLiteThreadExec::execQuery (const std::string& sql) const
{
    // Only one thread can use this db at the same time.
    boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);

    return SQLite::ExecQuery (_db, sql);
}




void 
SQLiteThreadExec::execUpdate (const std::string& sql) const
{
    // Only one thread can use this db at the same time.
    boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);
    SQLite::ExecUpdate (_db, sql);
}






}
}


