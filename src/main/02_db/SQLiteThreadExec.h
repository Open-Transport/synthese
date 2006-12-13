
/** SQLiteThreadExec class header.
	@file SQLiteThreadExec.h

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

#ifndef SYNTHESE_UTIL_SQLITETHREADEXEC_H
#define SYNTHESE_UTIL_SQLITETHREADEXEC_H

#include <vector>
#include <string>
#include <deque>
#include <map>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <sqlite/sqlite3.h>

#include "01_util/ThreadExec.h"
#include "01_util/UId.h"

#include "02_db/SQLiteUpdateHook.h"
#include "02_db/SQLiteResult.h"

namespace synthese
{
	namespace db
	{
		void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId);


		/** SQLite thread body class.

			This thread is an interface to the SQLite embedded database. It owns
			a dedicated SQLite db handle (connection) which is not released until
			the thread dies. Only one database file is managed per instance of this class.

			The interface wraps common SQLite operations into a C++ interface
			and wraps the result into STL containers.
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
		class SQLiteThreadExec : public synthese::util::ThreadExec
		{
		public:

		private:

			std::deque<SQLiteEvent> _eventQueue;

			const boost::filesystem::path _databaseFile;
			sqlite3* _db;  //!< SQLite db handle (connection).
			std::vector<SQLiteUpdateHook*> _hooks;   //!< Hooks to trigger on db update.
			
			boost::shared_ptr<boost::mutex> _hooksMutex; 
			boost::shared_ptr<boost::recursive_mutex> _queueMutex; 
			boost::shared_ptr<boost::recursive_mutex> _dbMutex; 

		public:
		    
			SQLiteThreadExec (const boost::filesystem::path& databaseFile);
			~SQLiteThreadExec ();

		    
			//! @name Query methods.
			//@{
			//@}


			//! @name Update methods.
			//@{
			void enqueueEvent (const SQLiteEvent& event);
			void postEvent (const SQLiteEvent& event) const;

		public:

			void registerUpdateHook (SQLiteUpdateHook* hook);

			//@}


			//! @name SQLite db access methods.
			//@{
			SQLiteResult execQuery (const std::string& sql) const;
			void execUpdate (const std::string& sql) const;
			//@}


			void initialize ();
			void loop ();
			void finalize ();

			friend void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId);

		};
	}
}

#endif

