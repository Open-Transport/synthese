#include "01_util/Conversion.h"
#include "01_util/Log.h"
#include "01_util/threads/Thread.h"
#include "01_util/Factory.h"

#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace db
	{

		void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId)
		{
			// WARNING : the update hook is invoked only when working with the connection
			// created inside the body of this thread (initialize).
			SQLiteQueueThreadExec* exec = (SQLiteQueueThreadExec*) userData;

			SQLiteEvent event;
			event.opType = opType;
			event.dbName = dbName;
			event.tbName = tbName;
			event.rowId = rowId;

			exec->enqueueEvent (event);

		}



		SQLiteQueueThreadExec::SQLiteQueueThreadExec (const boost::filesystem::path& databaseFile)
		    : SQLiteHandle ()
		    ,_databaseFile (databaseFile)
		    , _handle (0)
		    , _hooksMutex (new boost::mutex ())
		    , _queueMutex (new boost::recursive_mutex ())
		    , _handleMutex (new boost::recursive_mutex ())

		{
		}



		SQLiteQueueThreadExec::~SQLiteQueueThreadExec ()
		{
		}




		void 
		SQLiteQueueThreadExec::registerUpdateHook (SQLiteUpdateHook* hook)
		{
			boost::mutex::scoped_lock hooksLock (*_hooksMutex);

			// Lock db til register callback is executed (can use this db connection)
			boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);

			if (_handle != 0)
			{
			    throw SQLiteException ("Update hooks should have been registered before SQLite queue thread is started!");
			}

			_hooks.push_back (hook);

			/*
			  // NOTE : If we really need to add update hook on the fly, review this cos synchronous updates
			  // causes inifite loop (enqueued events but loop not started.)

			  if (_handle != 0)
			  {
			  // database handle has been initialized
			  // call the register callback directly
			  hook->registerCallback (this);
			}
			*/
		}




		void 
		SQLiteQueueThreadExec::postEvent (const SQLiteEvent& event)
		{
			boost::mutex::scoped_lock hooksLock (*_hooksMutex);

			for (std::vector<SQLiteUpdateHook*>::const_iterator it = _hooks.begin ();
			it != _hooks.end (); ++it)
			{
			(*it)->eventCallback (this, event);
			}
		    
		}





	    void 
	    SQLiteQueueThreadExec::enqueueEvent (const SQLiteEvent& event)
	    {
		boost::recursive_mutex::scoped_lock queueLock (*_queueMutex);
		_eventQueue.push_back (event);
	    }
	    
	    

	    
	    bool 
	    SQLiteQueueThreadExec::hasEnqueuedEvent () const
	    {
		boost::recursive_mutex::scoped_lock queueLock (*_queueMutex);
		return _eventQueue.empty () == false;
	    }




	    bool 
	    SQLiteQueueThreadExec::insideSQLiteQueueThread () const
	    {
		boost::thread currentThread;
		return ((*_initThread) == currentThread) ;
	    }


		void
		SQLiteQueueThreadExec::initialize()
		{
		    // The database cannot be updated til all the hooks have been initialized
		    // through their registerCallback.
		    boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
		    
		    // No hook can be added til the thread has finished its initialization.
		    boost::mutex::scoped_lock hooksLock (*_hooksMutex);

		    _initThread = new boost::thread (); // current thread of execution.
		    
		    // Open a persistent DB connection
		    // It is crucial that the db connection is created inside the init proc
		    // so that the connection is created in the caller thread.
		    _handle = SQLite::OpenHandle (_databaseFile);
		    
		    // Initialize each hooks
		    std::for_each (_hooks.begin(), _hooks.end (), 
				   std::bind2nd (std::mem_fun (&SQLiteUpdateHook::registerCallback), this));
		    
		    // Install the update hook
		    sqlite3_update_hook (_handle, &sqliteUpdateHook, this);
		}




		void
		SQLiteQueueThreadExec::loop()
		{
			// Lock the db to ensure that the handle is used by only one thread at
			// the same time!
			boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
			boost::recursive_mutex::scoped_lock queueLock (*_queueMutex);
			while (_eventQueue.empty () == false) 
			{
			    SQLiteEvent event = _eventQueue.front ();
			    
			    _eventQueue.pop_front ();
			    
			    try 
			    {
				postEvent (event);
			    }
			    catch (std::exception& e)
			    {
				Log::GetInstance().error ("Error while SQLite sync. In-memory data might be inconsistent.", e);
			    }
			    
			}


		}




		void
		SQLiteQueueThreadExec::finalize()
		{
		    SQLite::CloseHandle (_handle);
		    delete _initThread;
		}




		SQLiteResultSPtr 
		SQLiteQueueThreadExec::execQuery (const SQLiteStatementSPtr& statement, bool lazy)
		{
			// Only one thread can use this db at the same time.
			boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);

			// Note : lazy results can be used only inside the SQLite queue thread
			// (beacause of stepping which must be done in the same thread that created
			// statement). If this function is not called from inside the sqlite queue thread
			// (which is always the case for sync) we override lazy param and cache result anyway
			// so that calling thread will not be bothered by these threading considerations.
			// This limits a bit usage of lazy results... 
			// A good solution would be to implement a special event for queries which would be 
			// enqueued as well in the SQLiteQueueThread... see if really useful.
			if (insideSQLiteQueueThread () == false) lazy = false;
			return SQLiteHandle::execQuery (statement, lazy);
		}



	    
   	    SQLiteResultSPtr 
	    SQLiteQueueThreadExec::execQuery (const SQLData& sql, bool lazy)
	    {
		return this->execQuery (compileStatement (sql), lazy);
	    }
 



	    void
	    SQLiteQueueThreadExec::execUpdate (const SQLiteStatementSPtr& statement, bool asynchronous)
	    {
		    {
			// Only one thread can use this db at the same time.
			boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
			SQLiteHandle::execUpdate (statement);
		    }

		    if (asynchronous == false) 
		    {
			if (insideSQLiteQueueThread ()) 
			{
			    // We are in the "queue" thread 
			    // (because running monothreaded or in a unit test)
			    // So, just loop once to ensure the events are consumed.
			    ((SQLiteQueueThreadExec*) this)->loop ();
			}
			else
			{
			    while (hasEnqueuedEvent ()) 
			    {
				Thread::Sleep (1);			    
			    }
			}
		    }
	    }





	    
	    void 
	    SQLiteQueueThreadExec::execUpdate (const SQLData& sql, bool asynchronous)
	    {
		{
		    // Only one thread can use this db at the same time.
		    boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
		    SQLiteHandle::execUpdate (sql, asynchronous);
		}

		if (asynchronous == false) 
		{
		    if (insideSQLiteQueueThread ()) 
		    {
			// We are in the "queue" thread 
			// (because running monothreaded or in a unit test)
			// So, just loop once to ensure the events are consumed.
			((SQLiteQueueThreadExec*) this)->loop ();
		    }
		    else
		    {
			while (hasEnqueuedEvent ()) 
			{
			    Thread::Sleep (1);			    
			}
		    }
		}
	    }
	    
	    

	    SQLiteStatementSPtr 
	    SQLiteQueueThreadExec::compileStatement (const std::string& sql)
	    {
		// Important : even compiling requires the lock on the db handle.
		// Actually, ANY operation using a db handle must take the lock.
		boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
		return SQLiteHandle::compileStatement (sql);
	    }




	    bool 
	    SQLiteQueueThreadExec::isTransactionOpened ()
	    {
		boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
		return SQLiteHandle::isTransactionOpened ();
	    }
	    
	    
	    void 
	    SQLiteQueueThreadExec::beginTransaction (bool exclusive)
	    {
		boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
		return SQLiteHandle::beginTransaction (exclusive);
	    }
	    
	    
	    
	    void 
	    SQLiteQueueThreadExec::commitTransaction ()
	    {
		boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
		return SQLiteHandle::commitTransaction ();
	    }

	    
	    void 
	    SQLiteQueueThreadExec::rollbackTransaction ()
	    {
		boost::recursive_mutex::scoped_lock dbLock (*_handleMutex);
		return SQLiteHandle::rollbackTransaction ();
	    }
	    
	    
	}
}

