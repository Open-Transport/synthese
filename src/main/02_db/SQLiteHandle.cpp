#include "02_db/SQLiteHandle.h"

#include "02_db/SQLiteException.h"
#include "02_db/SQLiteLazyResult.h"
#include "02_db/SQLiteCachedResult.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"


using namespace synthese::util;
using namespace std;



namespace synthese
{

    namespace db
    {
	
	
	void cleanupHandle (sqlite3* hdl)
	{
	    int retc = sqlite3_close (hdl);
	    if (retc != SQLITE_OK)
	    {
		throw SQLiteException ("Cannot close SQLite handle (error=" + Conversion::ToString (retc) + ")");
	    }
	    // TODO clean update hook struct.
	}
	
	
	
	int sqliteBusyHandler (void* arg, int nbCalls)
	{
	    // Return a non-zero value so that a retry is made, waiting for SQLite not ot be busy anymore...
		cerr << "occupé";
		return 1;
	    
	}
	

	
	void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId)
	{

	    // WARNING : the update hook is invoked only when working with the connection
	    // created inside the body of this thread (initialize).
	    UpdateHookStruct* uhs = (UpdateHookStruct*) userData;
	    
	    SQLiteEvent event;
	    event.opType = opType;
	    event.dbName = dbName;
	    event.tbName = tbName;
	    event.rowId = rowId;
	    
	    uhs->events.push_back (event);
	}
	
	

	SQLiteHandle::SQLiteHandle (const boost::filesystem::path& databaseFile)
	    : _databaseFile (databaseFile)
	    , _handle (&cleanupHandle)
	    , _hooksMutex (new boost::mutex ())

	{
	}

	
	
	
	SQLiteHandle::~SQLiteHandle ()
	{
	}
	
	
	void 
	SQLiteHandle::registerUpdateHook (SQLiteUpdateHook* hook)
	{
	    boost::mutex::scoped_lock hooksLock (*_hooksMutex);
		    
	    _hooks.push_back (hook);
	    hook->registerCallback (this);
	}
	


	UpdateHookStruct* 
	SQLiteHandle::getUpdateHookStruct () const
	{
	    if (_updateHookStruct.get () == 0)
	    {
		_updateHookStruct.reset (new UpdateHookStruct ());
	    }
	    return _updateHookStruct.get ();
	}
	
	


	SQLiteResultSPtr 
	SQLiteHandle::execQuery (const SQLiteStatementSPtr& statement, bool lazy)
	{
//		cerr << "open" << statement->getSQL();
	    // lazy = false;
	    SQLiteResultSPtr result (new SQLiteLazyResult (statement));
	    if (lazy)
	    {
//			cerr << "close lazy" << statement->getSQL();
		return result;
	    }
	    else
	    {
		SQLiteCachedResult* cachedResult = new SQLiteCachedResult (result);
//		cerr << "close" << statement->getSQL();
		return SQLiteResultSPtr (cachedResult);
	    }
	}




	sqlite3* 
	SQLiteHandle::getHandle () const 
	{
	    if (_handle.get() == 0)
	    {
		
		sqlite3* handle;
		int retc = sqlite3_open (_databaseFile.string ().c_str (), &handle);
		if (retc != SQLITE_OK)
		{
		    throw SQLiteException ("Cannot open SQLite handle to " + 
					   _databaseFile.string () + "(error=" + Conversion::ToString (retc) + ")");
		}
		    
		// int 
		sqlite3_busy_handler(handle, &sqliteBusyHandler, 0);
		
		// std::cerr << " New handle ! " << handle << std::endl;
		sqlite3_update_hook (handle, &sqliteUpdateHook, getUpdateHookStruct ());
		_handle.reset (handle);
	    }
	    return _handle.get ();
	    
	}


	void 
	SQLiteHandle::execUpdate (const SQLiteStatementSPtr& statement)
	{
//	    cerr << "open" << statement->getSQL();
	    UpdateHookStruct* uhs = getUpdateHookStruct ();
	    uhs->events.clear ();

	    int retc = SQLITE_ROW;
	    while (retc == SQLITE_ROW)
	    {
		retc = sqlite3_step (statement->getStatement ());
	    }
	    if (retc != SQLITE_DONE)
	    {
		throw SQLiteException ("Error executing precompiled statement (error=" + Conversion::ToString (retc) + ")" + 
				       Conversion::ToTruncatedString (statement->getSQL ()));
	    }

	    // Call hooks!
	    const std::vector<SQLiteEvent>& events = uhs->events;
	    for (std::vector<SQLiteEvent>::const_iterator it = events.begin ();
		 it != events.end (); ++it)
	    {
		for (std::vector<SQLiteUpdateHook*>::const_iterator ith = _hooks.begin ();
		     ith != _hooks.end (); ++ith)
		{
		    (*ith)->eventCallback (this, *it);
		}
	    }
//		cerr << "close" << statement->getSQL();
    
	}


	void 
	SQLiteHandle::execUpdate (const SQLData& sql)
	{
//		cerr << "open" << sql;
	    UpdateHookStruct* uhs = getUpdateHookStruct ();
	    uhs->events.clear ();

	    // Do a batch execution (no precompilation since it can contains more than one 
	    // statement which is impossible to validate wihtout executing them one by one, given one database state)
	    assert (sql.size () > 0);
	    
	    char* errMsg = 0;
	    int retc = sqlite3_exec (getHandle (), 
				     sql.c_str (), 
				     0, 
				     0, &errMsg);
	    
	    if (retc != SQLITE_OK)
	    {
		std::string msg (errMsg);
		sqlite3_free (errMsg);
		
		throw SQLiteException ("Error executing batch update \"" + Conversion::ToTruncatedString (sql) + "\" : " + 
				       msg + " (error=" + Conversion::ToString (retc) + ")");
	    }

	    // Call hooks!
	    const std::vector<SQLiteEvent>& events = uhs->events;
	    for (std::vector<SQLiteEvent>::const_iterator it = events.begin ();
		 it != events.end (); ++it)
	    {
		for (std::vector<SQLiteUpdateHook*>::const_iterator ith = _hooks.begin ();
		     ith != _hooks.end (); ++ith)
		{
		    (*ith)->eventCallback (this, *it);
		}
	    }

//		cerr << "close" << sql;

	}


	    
	SQLiteStatementSPtr 
	SQLiteHandle::compileStatement (const SQLData& sql)
	{
	    return SQLiteStatementSPtr (new SQLiteStatement (*this, sql));
	}



    }
}

