////////////////////////////////////////////////////////////////////////////////
/// SQLiteHandle class header.
///	@file SQLiteHandle.h
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

#ifndef SYNTHESE_DB_SQLITEHANDLE_H
#define SYNTHESE_DB_SQLITEHANDLE_H

#include "SQLite.h"
#include "SQLiteUpdateHook.h"

#include <boost/thread/tss.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>



namespace synthese
{
	namespace db
	{


	    typedef struct
	    {
		sqlite3* handle;
		std::vector<SQLiteEvent> events;
		
	    } SQLiteTSS;
	    

		/** SQLite Handle class.

		    This class is aimed at providing interface to common sqlite3 operations on 
		    a sqlite3 handle.

		    IMPORTANT : to fasten execution of queries, it is crucial
		    to wrap them into transations (the exexcution time is about
		    60x shorter!!)

		    @ingroup m10
		*/
		class SQLiteHandle : public SQLite
		{

		private:

			  


		    const boost::filesystem::path _databaseFile;
		    mutable boost::thread_specific_ptr<SQLiteTSS> _tss;

		    std::vector<SQLiteUpdateHook*> _hooks;   //!< Hooks to trigger on db update.
		    boost::shared_ptr<boost::mutex> _hooksMutex; 
		    boost::shared_ptr<boost::recursive_mutex> _updateMutex; 

		protected:

		public:

			SQLiteHandle (const boost::filesystem::path& databaseFile);
			virtual ~SQLiteHandle ();
		    
			//! @name SQLite db access methods.
			//@{

			void registerUpdateHook (SQLiteUpdateHook* hook);

			virtual SQLiteStatementSPtr compileStatement (const SQLData& sql);

			virtual SQLiteResultSPtr execQuery (const SQLiteStatementSPtr& statement, bool lazy = false) ;

			virtual void execUpdate (const SQLiteStatementSPtr& statement) ;
			virtual void execUpdate (
				const SQLData& sql,
				boost::optional<SQLiteTransaction&> transaction = boost::optional<SQLiteTransaction&>()
			);

			static bool IsStatementComplete (const SQLData& sql);


			void callHooks (const SQLiteEvent& event);

			SQLiteTSS* getSQLiteTSS () const;

			//@}
		private:

			sqlite3* getHandle () const;


		};


	}
}

#endif

