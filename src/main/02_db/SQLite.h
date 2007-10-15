#ifndef SYNTHESE_DB_SQLITEDB_H
#define SYNTHESE_DB_SQLITEDB_H

#include "02_db/SQLiteStatement.h"
#include "02_db/SQLiteResult.h"



namespace synthese
{
	namespace db
	{

			  

		/** SQLite interface class.
		    @ingroup m02
		*/
		class SQLite
		{

		private:

		protected:

			SQLite ();
			virtual ~SQLite ();

		public:

		    
			//! @name SQLite db access methods.
			//@{

			virtual SQLiteStatementSPtr compileStatement (const SQLData& sql) = 0;

			virtual SQLiteResultSPtr execQuery (const SQLiteStatementSPtr& statement, bool lazy = false) = 0;
			virtual SQLiteResultSPtr execQuery (const SQLData& sql, bool lazy = false) ;

			virtual void execUpdate (const SQLiteStatementSPtr& statement) = 0;
			virtual void execUpdate (const SQLData& sql) = 0;

			/** Returns true if a transaction is already opened.
			    SQLite does not support nested transaction.
			*/
			// virtual bool isTransactionOpened () ;

			static bool IsStatementComplete (const SQLData& sql);

			static bool IsUpdateStatement (const SQLData& sql);

			//@}

		};


	}
}

#endif

