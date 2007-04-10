
/** SQLiteTableSync class header.
	@file SQLiteTableSync.h

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
#include "02_db/SQLiteTableFormat.h"


#define UPDATEABLE true;
#define NON_UPDATEABLE false;

namespace synthese
{
	namespace db
	{

		class SQLiteSync;
		class SQLiteResult;
		class SQLiteQueueThreadExec;

		/** @defgroup refLS Table synchronizers.
			@ingroup ref
		*/

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
			virtual void initAutoIncrement();

		public:

			SQLiteTableSync ( const std::string& tableName, 
					  bool allowInsert = true, 
					  bool allowRemove = true,
					  const std::string& triggerOverrideClause = "1",
					  bool ignoreCallbacksOnFirstSync = false);

			~SQLiteTableSync ();

			const std::string& getTableName () const;

			/** Returns the unique integer identifying a table.
				@return The unique integer identifying a table
			*/
			int getTableId () const;

			const SQLiteTableFormat& getTableFormat () const;

			bool getIgnoreCallbacksOnFirstSync () const;
			void setIgnoreCallbacksOnFirstSync (bool ignoreCallbacksOnFirstSync);

			void setEnableTriggers (bool enableTriggers);

			/** First synchronisation.
				This method is called when the synchronizer is created
				to synchronize it with pre-existing data in db.

				It creates ou updates all the needed tables and indexes.

				Note : It does not delete any useless table present in the database.
			*/
			void firstSync (const synthese::db::SQLiteQueueThreadExec* sqlite, 
					synthese::db::SQLiteSync* sync);
			
			/** This method can be overriden to invoke some code before
			    executing firstSync body.
			    Default implementation is doing nothing.
			*/
			virtual void beforeFirstSync (const SQLiteQueueThreadExec* sqlite, 
						      SQLiteSync* sync);

			/** This method can be overriden to invoke some code after
			    having executed firstSync body.
			    Default implementation is doing nothing.
			*/
			virtual void afterFirstSync (const SQLiteQueueThreadExec* sqlite, 
						     SQLiteSync* sync);

			virtual void rowsAdded (const SQLiteQueueThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows, bool isFirstSync = false) = 0;

			virtual void rowsUpdated (const SQLiteQueueThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows) = 0;

			virtual void rowsRemoved (const SQLiteQueueThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows) = 0;

		protected:

			void addTableColumn (const std::string& columnName, 
					const std::string& columnType, 
					bool updatable = true);

			/** Adds a multi-column index in the table description.
				@param columns Vector of column names
				@param name Name of the index. Optional : if not specified or empty, the name is the concatenation of the columns names
			*/
			void addTableIndex(const std::vector<std::string>& columns, std::string name = "");

			/** Adds a single column index in the table description.
				@param column column name
				@param name Name of the index. Optional : if not specified or empty, the name is identical to the column name.
			*/
			void addTableIndex(const std::string& column, std::string name = "");

		protected:

			static int ParseTableId (const std::string& tableName);


		private:

			std::string getTriggerOverrideClause () const;


			/** Creates table in SQLite db according to this class
			 * table format.
			 *
			 * @param tableSchema Required table schema.
			 */
			void createTable (const synthese::db::SQLiteQueueThreadExec* sqlite,
					  const std::string& tableSchema,
					  const std::string& triggerNoInsert,
					  const std::string& triggerNoRemove,
					  const std::string& triggerNoUpdate);

			/** Adapts table in SQLite db to conform to this class 
			 * table format.
			 * Right now, only new column addition/insertion is supported.
			 * Any other change to table schema is not supported yet.
			 *
			 * @param tableSchema Required table schema.
			 * @param dbSchema Actual table schema in db.
			 */
			void adaptTable (const synthese::db::SQLiteQueueThreadExec* sqlite,
					 const std::string& tableSchema,
					 const std::string& triggerNoInsert,
					 const std::string& triggerNoRemove,
					 const std::string& triggerNoUpdate);
			

		public:

			/** Creates the SQL statement to create a table in db
			 * given a certain format.
			 */
			static std::string CreateSQLSchema (const std::string& tableName,
							    const SQLiteTableFormat& format);

			/** Creates the SQL statement to crate an index in the database given a certain format.
				@param tableName The table name
				@param format The format of the table containing the indexes description
				@return std::string The SQL statement
				@author Hugues Romain
				@date 2007

				Note : the real index name in the database is the concatenation of the table name and the name specified in the table format separated by a _ character.
			*/
			static std::string CreateIndexSQLSchema(const std::string& tableName, const SQLiteTableIndexFormat& format);
			
			static std::string CreateTriggerNoInsert (
			    const std::string& tableName,
			    const std::string& triggerOverrideClause);
			
			static std::string CreateTriggerNoRemove (
			    const std::string& tableName,
			    const std::string& triggerOverrideClause);
			
			static std::string CreateTriggerNoUpdate (
			    const std::string& tableName,
			    const SQLiteTableFormat& format,
			    const std::string& triggerOverrideClause);
			



			static std::vector<std::string> 
			    GetTableColumnsDb (const synthese::db::SQLiteQueueThreadExec* sqlite,
					       const std::string& tableName);

			static std::string GetSQLSchemaDb (const synthese::db::SQLiteQueueThreadExec* sqlite,
							   const std::string& tableName);
			
			static std::string GetTriggerNoInsertDb (
			    const synthese::db::SQLiteQueueThreadExec* sqlite,
			    const std::string& tableName);

			static std::string GetTriggerNoRemoveDb (
			    const synthese::db::SQLiteQueueThreadExec* sqlite,
			    const std::string& tableName);

			static std::string GetTriggerNoUpdateDb (
			    const synthese::db::SQLiteQueueThreadExec* sqlite,
			    const std::string& tableName);

			/** Builds the name of the index in the sqlite master table.
				@param tableName Name of the table which belongs the index
				@param name Name of the index within the table
				@return the full name
				@author Hugues Romain
				@date 2007
				
			*/
			static std::string getIndexDBName(const std::string& tableName, const std::string& name);
		};
	}
}

#endif
