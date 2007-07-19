#ifndef SYNTHESE_DB_SQLITEHANDLE_H
#define SYNTHESE_DB_SQLITEHANDLE_H

#include "02_db/SQLite.h"



namespace synthese
{
	namespace db
	{

		/** SQLite Handle class.

		    This class is aimed at providing interface to common sqlite3 operations on 
		    a sqlite3 handle.

		    IMPORTANT : to fasten execution of queries, it is crucial
		    to wrap them into transations (the exexcution time is about
		    60x shorter!!)

		    @ingroup m02
		*/
		class SQLiteHandle
		{
		protected:

			SQLiteHandle ();
			virtual ~SQLiteHandle ();
		public:


		    
			//! @name SQLite db access methods.
			//@{

			virtual sqlite3* getHandle () = 0;

			virtual SQLiteResult execQuery (const std::string& sql) ;
			virtual void execUpdate (const std::string& sql, bool asynchronous = false) ;

			/** Returns true if a transaction is already opened.
			    SQLite does not support nested transaction.
			*/
			virtual bool isTransactionOpened () ;
			
			/* Begins an exclusive transaction on the SQLite db. Exclusive means no read nor write
			   until the transaction is commited. If exclusive is false, it means that read is still
			   possible before the transaction is completed.
			*/
			virtual void beginTransaction (bool exclusive = false);
			virtual void commitTransaction ();
			
			virtual SQLiteStatement prepareStatement (const std::string& sql);
			virtual void finalizeStatement (const SQLiteStatement& statement);

			//@}

		};
	}
}

#endif

