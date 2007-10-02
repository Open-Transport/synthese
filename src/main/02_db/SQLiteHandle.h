#ifndef SYNTHESE_DB_SQLITEHANDLE_H
#define SYNTHESE_DB_SQLITEHANDLE_H

#include "02_db/SQLite.h"
#include "02_db/SQLiteUpdateHook.h"

#include <boost/shared_ptr.hpp>



namespace synthese
{
	namespace db
	{

		typedef struct
		{
		    std::vector<SQLiteEvent> events;
		} UpdateHookStruct;
			  


		/** SQLite Handle class.

		    This class is aimed at providing interface to common sqlite3 operations on 
		    a sqlite3 handle.

		    IMPORTANT : to fasten execution of queries, it is crucial
		    to wrap them into transations (the exexcution time is about
		    60x shorter!!)

		    @ingroup m02
		*/
		class SQLiteHandle : public SQLite
		{

		private:
		    
		    const boost::filesystem::path _databaseFile;
		    mutable boost::thread_specific_ptr<sqlite3> _handle;
		    mutable boost::thread_specific_ptr<UpdateHookStruct> _updateHookStruct;

		    std::vector<SQLiteUpdateHook*> _hooks;   //!< Hooks to trigger on db update.
		    boost::shared_ptr<boost::mutex> _hooksMutex; 

		protected:

		public:

			SQLiteHandle (const boost::filesystem::path& databaseFile);
			virtual ~SQLiteHandle ();
		    
			//! @name SQLite db access methods.
			//@{

			sqlite3* getHandle () const;

			void registerUpdateHook (SQLiteUpdateHook* hook);

			virtual SQLiteStatementSPtr compileStatement (const SQLData& sql);

			virtual SQLiteResultSPtr execQuery (const SQLiteStatementSPtr& statement, bool lazy = false) ;

			virtual void execUpdate (const SQLiteStatementSPtr& statement) ;
			virtual void execUpdate (const SQLData& sql);

			static bool IsStatementComplete (const SQLData& sql);


			//@}
		private:
			UpdateHookStruct* getUpdateHookStruct () const;

		};


	}
}

#endif

