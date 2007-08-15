
/** SQLiteQueueThreadExec class header.
	@file SQLiteQueueThreadExec.h

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

#ifndef SYNTHESE_DB_SQLITEQUEUETHREADEXEC_H
#define SYNTHESE_DB_SQLITEQUEUETHREADEXEC_H


#include "02_db/SQLiteHandle.h"

#include "02_db/SQLiteUpdateHook.h"

#include "01_util/threads/ThreadExec.h"
#include "01_util/UId.h"

#include <vector>
#include <string>
#include <deque>
#include <map>

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace db
	{
		void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId);


		/** SQLite thread body class.

			This thread is an interface to the SQLite embedded database. It owns
			a dedicated SQLite db handle (connection) which is not released until
			the thread dies. Only one database file is managed per instance of this class.

			The other reason for this interface to exist is proper handling of update hooks
			triggered on db modification. To do so, this class manages internally a queue
			of db events which are treated in FIFO mode inside the body of the thread. 
		    
			This guarantees that the following SQLite constraints are satisfied :
			- No database access is performed inside the body of sql3_update_hook 

			It is safe to use the db handle created by the exec thread from another thread
			(by calling execUpdate or execQuery) because this class guarantees that the db handle
			is used by one thread at the same time.

			@ingroup m02
		*/
		class SQLiteQueueThreadExec : public SQLiteHandle, public synthese::util::ThreadExec
		{
		public:

		private:

			std::deque<SQLiteEvent> _eventQueue;

			boost::thread* _initThread;

			const boost::filesystem::path _databaseFile;
			sqlite3* _handle;  //!< SQLite db handle (connection).
			std::vector<SQLiteUpdateHook*> _hooks;   //!< Hooks to trigger on db update.
			
			boost::shared_ptr<boost::mutex> _hooksMutex; 
			boost::shared_ptr<boost::recursive_mutex> _queueMutex; 
			boost::shared_ptr<boost::recursive_mutex> _handleMutex; 

		public:
		    
			SQLiteQueueThreadExec (const boost::filesystem::path& databaseFile);
			~SQLiteQueueThreadExec ();

		    
			//! @name Query methods.
			//@{
			bool hasEnqueuedEvent () const;

			/* Returns whether or not the thread calling this function is
			   the SQLite queue thread.
			*/
			bool insideSQLiteQueueThread () const;
			//@}

			//! @name Update methods.
			//@{
			void enqueueEvent (const SQLiteEvent& event);
			void postEvent (const SQLiteEvent& event);

		public:

			void registerUpdateHook (SQLiteUpdateHook* hook);

			//@}

		public:

			//! @name SQLite db access methods.
			//@{
			sqlite3* getHandle () { return _handle; }

			SQLiteStatementSPtr compileStatement (const SQLData& sql);

			SQLiteResultSPtr execQuery (const SQLiteStatementSPtr& statement, bool lazy = true) ;
			SQLiteResultSPtr execQuery (const SQLData& sql, bool lazy = true) ;

			void execUpdate (const SQLiteStatementSPtr& statement, bool asynchronous = false) ;
			void execUpdate (const SQLData& sql, bool asynchronous = false);

			bool isTransactionOpened () ;
			void beginTransaction (bool exclusive = false);
			void commitTransaction ();
			void rollbackTransaction ();

			//@}


			void initialize ();
			void loop ();
			void finalize ();

			friend void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId);

		};
	}
}

#endif

