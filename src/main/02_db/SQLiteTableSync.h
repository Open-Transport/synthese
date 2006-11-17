#ifndef SYNTHESE_DB_SQLITETABLESYNC_H
#define SYNTHESE_DB_SQLITETABLESYNC_H



#include <string>
#include <vector>
#include <iostream>

#include "01_util/Factorable.h"
#include "01_util/UId.h"

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

			const std::string _tableName;
			SQLiteTableFormat _tableFormat;

		public:

			SQLiteTableSync ( const std::string& tableName, 
					bool allowInsert = true, 
					bool allowRemove = true,
					const std::string& triggerOverrideClause = "1");

			~SQLiteTableSync ();

			const std::string& getTableName () const;

			/** Returns the unique integer identifying a table.
			*/
			int getTableId () const;

			const SQLiteTableFormat& getTableFormat () const;

			uid encodeUId (int gridId, int gridNodeId, long objectId);

			/** This method is called when the synchronizer is created
			to sychronize it with pre-existing data in db.
			*/
			void firstSync (const synthese::db::SQLiteThreadExec* sqlite, 
					synthese::db::SQLiteSync* sync);

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

		};
	}
}
#endif
