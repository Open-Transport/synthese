
/** SQLiteQueueThreadExec class Implementation.
	@file SQLiteQueueThreadExec.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "01_util/Conversion.h"
#include "01_util/Log.h"
#include "01_util/Thread.h"
#include "01_util/Factory.h"

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

//		boost::mutex eventQueueMutex; 

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
		: _databaseFile (databaseFile)
		, _db (0)
		, _hooksMutex (new boost::mutex ())
		, _queueMutex (new boost::recursive_mutex ())
		, _dbMutex (new boost::recursive_mutex ())

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
			boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);

			if (_db != 0)
			{
			    throw SQLiteException ("Update hooks should have been registered before SQLite queue thread is started!");
			}

			_hooks.push_back (hook);

			/*
			  // NOTE : If we really need to add update hook on the fly, review this cos synchronous updates
			  // causes inifite loop (enqueued events but loop not started.)

			  if (_db != 0)
			  {
			  // database handle has been initialized
			  // call the register callback directly
			  hook->registerCallback (this);
			}
			*/
		}




		void 
		SQLiteQueueThreadExec::postEvent (const SQLiteEvent& event) const
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
		    boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);
		    
		    // No hook can be added til the thread has finished its initialization.
		    boost::mutex::scoped_lock hooksLock (*_hooksMutex);

		    _initThread = new boost::thread (); // current thread of execution.
		    
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
		SQLiteQueueThreadExec::loop()
		{
			// Lock the db to ensure that the handle is used by only one thread at
			// the same time!
			boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);
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
		    SQLite::CloseConnection (_db);
		    delete _initThread;
		}




		SQLiteResult 
		SQLiteQueueThreadExec::execQuery (const std::string& sql) const
		{
			// Only one thread can use this db at the same time.
			boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);

			return SQLite::ExecQuery (_db, sql);
		}




		void 
		SQLiteQueueThreadExec::execUpdate (const std::string& sql, bool asynchronous) const
		{
		    {
			// Only one thread can use this db at the same time.
			boost::recursive_mutex::scoped_lock dbLock (*_dbMutex);
			SQLite::ExecUpdate (_db, sql);
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
	    SQLiteQueueThreadExec::beginTransaction (bool exclusive)
	    {
		SQLite::BeginTransaction (_db, exclusive);
	    }
	    

	    
	    void 
	    SQLiteQueueThreadExec::commitTransaction ()
	    {
		SQLite::CommitTransaction (_db);
	    }


	    
	    SQLiteStatement 
	    SQLiteQueueThreadExec::prepareStatement (const std::string& sql)
	    {
		SQLite::PrepareStatement (_db, sql);
	    }


	    void 
	    SQLiteQueueThreadExec::finalizeStatement (const SQLiteStatement& statement)
	    {
		SQLite::FinalizeStatement (statement);
	    }

	    
	}
}

