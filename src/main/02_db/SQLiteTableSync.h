#ifndef SYNTHESE_DB_SQLITETABLESYNC_H
#define SYNTHESE_DB_SQLITETABLESYNC_H


#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "01_util/Factorable.h"
#include "01_util/UId.h"

#include "02_db/DBModule.h"

#define UPDATEABLE true;
#define NON_UPDATEABLE false;

namespace synthese
{
	namespace db
	{

		class SQLiteSync;
		class SQLiteResult;
		class SQLiteThreadExec;

		typedef struct {
		std::string name;
		std::string type;
		bool updatable;
		} SQLiteTableColumnFormat;

		typedef std::vector<SQLiteTableColumnFormat> SQLiteTableFormat;

		/** Base class for an SQLite table synchronizer.
			By convention, the table name must always start with the t letter
			followed by a unique 3 digits integer (SQLite does not allow ids starting with number).

			@ingroup m02
		*/
		class SQLiteTableSync : public util::Factorable
		{
		private:

			const bool _allowInsert;
			const bool _allowRemove;
			const std::string _triggerOverrideClause;
			bool _ignoreCallbacksOnFirstSync;
			bool _enableTriggers;

			const std::string _tableName;
			SQLiteTableFormat _tableFormat;

		protected:
			virtual void initAutoIncrement(const SQLiteThreadExec* sqlite);

		public:

			SQLiteTableSync ( const std::string& tableName, 
					  bool allowInsert = true, 
					  bool allowRemove = true,
					  const std::string& triggerOverrideClause = "1",
					  bool ignoreCallbacksOnFirstSync = false);

			~SQLiteTableSync ();

			const std::string& getTableName () const;

			/** Returns the unique integer identifying a table.
			*/
			int getTableId () const;

			const SQLiteTableFormat& getTableFormat () const;

			bool getIgnoreCallbacksOnFirstSync () const;
			void setIgnoreCallbacksOnFirstSync (bool ignoreCallbacksOnFirstSync);

			void setEnableTriggers (bool enableTriggers);

			/** This method is called when the synchronizer is created
			to synchronize it with pre-existing data in db.
			*/
			void firstSync (const synthese::db::SQLiteThreadExec* sqlite, 
					synthese::db::SQLiteSync* sync);
			
			/** This method can be overriden to invoke some code before
			    executing firstSync body.
			    Default implementation is doing nothing.
			*/
			virtual void beforeFirstSync (const SQLiteThreadExec* sqlite, 
						      SQLiteSync* sync);

			/** This method can be overriden to invoke some code after
			    having executed firstSync body.
			    Default implementation is doing nothing.
			*/
			virtual void afterFirstSync (const SQLiteThreadExec* sqlite, 
						     SQLiteSync* sync);

			virtual void rowsAdded (const SQLiteThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows) = 0;

			virtual void rowsUpdated (const SQLiteThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows) = 0;

			virtual void rowsRemoved (const SQLiteThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows) = 0;

		protected:

			void addTableColumn (const std::string& columnName, 
					const std::string& columnType, 
					bool updatable = true);

		protected:

			static int ParseTableId (const std::string& tableName);


		private:

			std::string getTriggerOverrideClause () const;

			/** Creates table in SQLite db according to this class
			 * table format.
			 */
			void createTable (const synthese::db::SQLiteThreadExec* sqlite);

			/** Adapts table in SQLite db to conform to this class 
			 * table format.
			 * Right now, only new column addition/insertion is supported.
			 * Any other change to table schema is not supported yet.
			 */
			void alterTable (const synthese::db::SQLiteThreadExec* sqlite);



		};
	}
}
#endif

