////////////////////////////////////////////////////////////////////////////////
/// SQLiteHandle class implementation.
///	@file SQLiteHandle.cpp
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "SQLiteHandle.h"
#include "SQLiteException.h"
#include "SQLiteLazyResult.h"
#include "SQLiteCachedResult.h"
#include "SQLiteTransaction.h"

#include "Conversion.h"
#include "Log.h"

#include <algorithm>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;	

    namespace db
    {
		
		
		void cleanupTSS (SQLiteTSS* tss)
		{
			int retc = sqlite3_close (tss->handle);
			if (retc != SQLITE_OK)
			{
			throw SQLiteException ("Cannot close SQLite handle (error=" + Conversion::ToString (retc) + ")");
			}
			delete tss;
		}
		
		
		
		int sqliteBusyHandler (void* arg, int nbCalls)
		{
			// Return a non-zero value so that a retry is made, waiting for SQLite not ot be busy anymore...
			return 1;
			
		}
		

		
		void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId)
		{

			// WARNING : the update hook is invoked only when working with the connection
			// created inside the body of this thread (initialize).
			SQLiteTSS* tss = (SQLiteTSS*) userData;
		    
			SQLiteEvent event;
			event.opType = opType;
			event.dbName = dbName;
			event.tbName = tbName;
			event.rowId = rowId;
		    
			tss->events.push_back (event);
		}
		

		void sqliteRollbackHook (void* arg)
		{
			SQLiteTSS* tss = (SQLiteTSS*) arg;
			tss->events.clear ();
		}



		void 
		SQLiteHandle::callHooks (const SQLiteEvent& event)
		{
			// Call hooks!
			for (std::vector<SQLiteUpdateHook*>::const_iterator ith = _hooks.begin ();
			 ith != _hooks.end (); ++ith)
			{
			(*ith)->eventCallback (this, event);
			}
		    
		}

		

		SQLiteHandle::SQLiteHandle (const boost::filesystem::path& databaseFile)
			: _databaseFile (databaseFile)
			, _tss (&cleanupTSS)
			, _hooksMutex (new boost::mutex ())
			, _updateMutex (new boost::recursive_mutex ())
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
		


		SQLiteTSS* 
		SQLiteHandle::getSQLiteTSS () const
		{
			if (_tss.get () == 0)
			{
			// Create the sqlite handle
			
			sqlite3* handle;
			int retc = sqlite3_open (_databaseFile.string ().c_str (), &handle);
			if (retc != SQLITE_OK)
			{
				throw SQLiteException ("Cannot open SQLite handle to " + 
						   _databaseFile.string () + "(error=" + Conversion::ToString (retc) + ")");
			}
			
			// int 
			sqlite3_busy_handler(handle, &sqliteBusyHandler, 0);

			//lint --e{429}
			SQLiteTSS* tss = new SQLiteTSS ();

			sqlite3_update_hook (handle, &sqliteUpdateHook, tss);
			sqlite3_rollback_hook (handle, &sqliteRollbackHook, tss);
			
			tss->handle = handle;

			_tss.reset (tss);
			}
			return _tss.get ();
		}
		
		


		SQLiteResultSPtr 
		SQLiteHandle::execQuery (const SQLiteStatementSPtr& statement, bool lazy)
		{
//			assert (lazy == false);
//			lazy = false;
			SQLiteResultSPtr result (new SQLiteLazyResult (statement));
			if (lazy)
			{
			return result;
			}
			else
			{
			SQLiteCachedResult* cachedResult = new SQLiteCachedResult (result);
			return SQLiteResultSPtr (cachedResult);
			}
		}




		sqlite3* 
		SQLiteHandle::getHandle () const 
		{
			return getSQLiteTSS ()->handle;
		}


		void 
		SQLiteHandle::execUpdate (const SQLiteStatementSPtr& statement)
		{
			// Lock this method so that no database update can start before hooks
			// have finished their execution. The mutex is recursive so that
			// an update can still be called inside hook callback.
			boost::recursive_mutex::scoped_lock lock (*_updateMutex);
			SQLiteTSS* tss = getSQLiteTSS ();
			tss->events.clear ();

			if (statement->insideOwnerThread () == false)
			{
			throw SQLiteException ("SQLiteStatement called outside its creation thread is forbidden.");
			}

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

			const std::vector<SQLiteEvent>& events = tss->events;
			for (std::vector<SQLiteEvent>::const_iterator it = events.begin ();
			 it != events.end (); ++it)
			{
			callHooks (*it);
			}
		}


		void SQLiteHandle::execTransaction(
			const SQLiteTransaction& transaction
		){
			// Lock this method so that no database update can start before hooks
			// have finished their execution. The mutex is recursive so that
			// an update can still be called inside hook callback.
			boost::recursive_mutex::scoped_lock lock (*_updateMutex);

			SQLiteTSS* tss = getSQLiteTSS ();
			tss->events.clear ();

			char* errMsg = 0;

			int retc = sqlite3_exec(getHandle(),"BEGIN TRANSACTION;", 0,	0, &errMsg);
			if (retc != SQLITE_OK)
			{
				std::string msg (errMsg);
				sqlite3_free (errMsg);

				throw SQLiteException ("Error executing batch update when opening transaction : " + 
					msg + " (error=" + Conversion::ToString (retc) + ")");
			}

			try
			{
				BOOST_FOREACH(const SQLiteTransaction::Queries::value_type& sql, transaction.getQueries())
				{
					retc = sqlite3_exec (getHandle (), 
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
				}
			}
			catch(...)
			{
				retc = sqlite3_exec(getHandle(),"COMMIT;", 0,	0, &errMsg);
				if (retc != SQLITE_OK)
				{
					std::string msg (errMsg);
					sqlite3_free (errMsg);

					throw SQLiteException ("Error executing batch update when commiting transaction, database may be locked : " + 
						msg + " (error=" + Conversion::ToString (retc) + ")");
				}
			}

			retc = sqlite3_exec(getHandle(),"COMMIT;", 0,	0, &errMsg);
			if (retc != SQLITE_OK)
			{
				std::string msg (errMsg);
				sqlite3_free (errMsg);

				throw SQLiteException ("Error executing batch update when commiting transaction, database may be locked : " + 
					msg + " (error=" + Conversion::ToString (retc) + ")");
			}

			BOOST_FOREACH(const SQLiteEvent& event, tss->events)
			{
				callHooks(event);
			}
		}



		void SQLiteHandle::execUpdate(
			const SQLData& sql,
			optional<SQLiteTransaction&> transaction
		){
			if(transaction)
			{
				transaction->add(sql);
				return;
			}

			// Lock this method so that no database update can start before hooks
			// have finished their execution. The mutex is recursive so that
			// an update can still be called inside hook callback.
			boost::recursive_mutex::scoped_lock lock (*_updateMutex);

			// Do a batch execution (no precompilation since it can contains more than one 
			// statement which is impossible to validate wihtout executing them one by one, given one database state)
			assert (sql.size () > 0);
		    
			SQLiteTSS* tss = getSQLiteTSS ();
			tss->events.clear ();

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

			const std::vector<SQLiteEvent>& events = tss->events;
			for (std::vector<SQLiteEvent>::const_iterator it = events.begin ();
			 it != events.end (); ++it)
			{
			callHooks (*it);
			}
		}


		    
		SQLiteStatementSPtr 
		SQLiteHandle::compileStatement (const SQLData& sql)
		{
			sqlite3_stmt* st;
		    
			int retc = sqlite3_prepare_v2 (getHandle (), 
						   sql.c_str (), sql.length (), &st, 0);

			if (retc != SQLITE_OK)
			{
			throw SQLiteException ("Error compiling \"" + sql + "\" (error=" + Conversion::ToString (retc) + ")");
			}
			return SQLiteStatementSPtr (new SQLiteStatement (st, sql));
		}
    }
}

