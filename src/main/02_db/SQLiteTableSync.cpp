
/** SQLiteTableSync class implementation.
	@file SQLiteTableSync.cpp

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

#include <sstream>

#include <sqlite/sqlite3.h>

#include <boost/thread/mutex.hpp>

#include "01_util/Conversion.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteException.h"

using namespace std;

namespace synthese
{
    using namespace util;

    namespace db
    {
	//		std::map<int, int> SQLiteTableSync::_autoIncrementValues;

		SQLiteTableSync::SQLiteTableSync ( const std::string& tableName,
						   bool allowInsert, 
						   bool allowRemove,
						   const std::string& triggerOverrideClause,
						   bool ignoreCallbacksOnFirstSync
			)
			: _tableName (tableName)
			, _allowInsert (allowInsert)
			, _allowRemove (allowRemove)
			, _triggerOverrideClause (triggerOverrideClause)
			, _ignoreCallbacksOnFirstSync (ignoreCallbacksOnFirstSync)
			, _enableTriggers (true)
		{ }



		SQLiteTableSync::~SQLiteTableSync ()
		{

		}



		void 
		SQLiteTableSync::firstSync (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						synthese::db::SQLiteSync* sync)
		{
			// Pre-init phase
			beforeFirstSync (sqlite, sync);


			// Check if the table already exists
			std::string sql = "SELECT * FROM SQLITE_MASTER WHERE name='" + _tableName + "' AND type='table'";

			SQLiteResult res = sqlite->execQuery (sql);
			std::string tableSchema = CreateSQLSchema (_tableName, _tableFormat);

			std::string triggerNoInsert = _allowInsert ? "" : 
			CreateTriggerNoInsert (_tableName, getTriggerOverrideClause ());

			std::string triggerNoRemove = _allowRemove ? "" : 
			CreateTriggerNoRemove (_tableName, getTriggerOverrideClause ());

			std::string triggerNoUpdate = (_tableFormat.hasNonUpdatableColumn () == false) ? "" : 
			CreateTriggerNoUpdate (_tableName, _tableFormat, getTriggerOverrideClause ());
		    
		    
			if (res.getNbRows () == 0)
			{
			// If not, create it...
			createTable (sqlite, 
					 tableSchema, 
					 triggerNoInsert, 
					 triggerNoRemove,
					 triggerNoUpdate);
			}
			else
			{
			// ...otherwise, tries to adapt it.
			adaptTable (sqlite, 
					tableSchema, 
					triggerNoInsert, 
					triggerNoRemove,
					triggerNoUpdate);
			}
				
			// Callbacks according to what already exists in the table.
			if (_ignoreCallbacksOnFirstSync == false)
			{
			SQLiteResult result = sqlite->execQuery ("SELECT * FROM " + _tableName);
			rowsAdded (sqlite, sync, result);
			}

			initAutoIncrement ();

			// Indexes
			SQLiteTableIndexMap indexes = _tableFormat.getTableIndexes();
			for (SQLiteTableIndexMap::const_iterator it = indexes.begin(); it != indexes.end(); ++it)
			{
				// Search if the index exists
				stringstream sql;
				sql << "SELECT sql FROM SQLITE_MASTER WHERE name='" << getIndexDBName(_tableName, it->first)
					<< "' AND type='index'";

				SQLiteResult res = sqlite->execQuery (sql.str());

				// The index already exists
				if (res.getNbRows())
				{
					// The index already exists and is identical : do nothing
					if (res.getColumn(0, "sql") == CreateIndexSQLSchema(_tableName, *it))
						continue;

					// Drop the old index
					stringstream drop;
					drop << "DROP INDEX " << getIndexDBName(_tableName, it->first);
					sqlite->execUpdate(drop.str());
				}

				// Creation of the index
				sqlite->execUpdate(CreateIndexSQLSchema(_tableName, *it));
			}


			// Post-init phase
			afterFirstSync (sqlite, sync);
				

		}

			    

		void 
		SQLiteTableSync::beforeFirstSync (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						  synthese::db::SQLiteSync* sync)
		{
		}

		void 
		SQLiteTableSync::afterFirstSync (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						 synthese::db::SQLiteSync* sync)
		{
		}


		const std::string& 
		SQLiteTableSync::getTableName () const
		{
			return _tableName;
		}


		int 
		SQLiteTableSync::getTableId () const
		{
			return ParseTableId (_tableName);
		}






		const SQLiteTableFormat& 
		SQLiteTableSync::getTableFormat () const
		{
			return _tableFormat;
		}





		void 
		SQLiteTableSync::addTableColumn (const std::string& columnName, 
						 const std::string& columnType,
						 bool updatable)
		{
			_tableFormat.addTableColumn (columnName, columnType, updatable);
		}




		int 
		SQLiteTableSync::ParseTableId (const std::string& tableName)
		{
			return Conversion::ToInt (tableName.substr (1, 4));
		}

		void SQLiteTableSync::initAutoIncrement()
		{

		}

		    


		bool 
		SQLiteTableSync::getIgnoreCallbacksOnFirstSync () const
		{
			return _ignoreCallbacksOnFirstSync;
		}
			

		void 
		SQLiteTableSync::setIgnoreCallbacksOnFirstSync (bool ignoreCallbacksOnFirstSync)
		{
			_ignoreCallbacksOnFirstSync = ignoreCallbacksOnFirstSync;
		}


		    

		void 
		SQLiteTableSync::setEnableTriggers (bool enableTriggers)
		{
			_enableTriggers = enableTriggers;
		}


		    
		std::string 
		SQLiteTableSync::getTriggerOverrideClause () const
		{
			return _enableTriggers ? _triggerOverrideClause : "0";
		}

		

		void 
		SQLiteTableSync::createTable (const synthese::db::SQLiteQueueThreadExec* sqlite,
						  const std::string& tableSchema,
						  const std::string& triggerNoInsert,
						  const std::string& triggerNoRemove,
						  const std::string& triggerNoUpdate)
		{
			const SQLiteTableFormat& format = getTableFormat ();

			// Create the table if it does not already exist.
			sqlite->execUpdate (tableSchema);

			// Insert some triggers to prevent unallowed insert/update/remove operations
			sqlite->execUpdate (triggerNoInsert);
			sqlite->execUpdate (triggerNoRemove);
			sqlite->execUpdate (triggerNoUpdate);
				
		}




		std::string 
		SQLiteTableSync::CreateSQLSchema (const std::string& tableName,
						  const SQLiteTableFormat& format)
		{
			std::string sql = "CREATE TABLE " + tableName + " (";
			sql.append (format.getTableColumn(0).name).append (" ")
			.append (format.getTableColumn(0).type).append (" UNIQUE PRIMARY KEY");

			for (int i=1; i< (int) format.getTableColumnCount (); ++i)
			{
			sql.append (", ").append (format.getTableColumn (i).name)
				.append (" ").append (format.getTableColumn (i).type);
			}
			sql.append (")");
			return sql;
		}



		std::string 
		SQLiteTableSync::CreateTriggerNoInsert (const std::string& tableName,
							const std::string& triggerOverrideClause)
		{
			std::string sql = "CREATE TRIGGER " ;
			sql.append (tableName + "_no_insert");
			sql.append (" BEFORE INSERT ON " + tableName);
			sql.append (" BEGIN SELECT RAISE (ABORT, 'Insertion in " + tableName 
				+ " is forbidden.') WHERE " + triggerOverrideClause + "; END");
			return sql;
		}
				
			



		std::string 
		SQLiteTableSync::CreateTriggerNoRemove (const std::string& tableName,
							const std::string& triggerOverrideClause)
		{
			std::string sql = "CREATE TRIGGER " ;
			sql.append (tableName + "_no_remove");
			sql.append (" BEFORE DELETE ON " + tableName);
			sql.append (" BEGIN SELECT RAISE (ABORT, 'Deletion in " + tableName 
				+ " is forbidden.') WHERE " + triggerOverrideClause + "; END");
			return sql;
		}


				
		std::string 
		SQLiteTableSync::CreateTriggerNoUpdate (const std::string& tableName,
							const SQLiteTableFormat& format,
							const std::string& triggerOverrideClause)
		{
			std::vector<std::string> nonUpdatableColumns;
			for (int i=0; i<format.getTableColumnCount (); ++i)
			{
			SQLiteTableColumnFormat columnFormat = format.getTableColumn (i);
			if (columnFormat.updatable == false)
			{
				nonUpdatableColumns.push_back (columnFormat.name);
			}
			}

		    
			if (nonUpdatableColumns.empty ()) return "";

			std::string sql = "CREATE TRIGGER " ;
			sql.append (tableName + "_no_update");
			sql.append (" BEFORE UPDATE OF ");
			std::string columnList;
			for (int i=0; i< (int) nonUpdatableColumns.size (); ++i)
			{
			columnList.append (nonUpdatableColumns[i]);
			if (i != nonUpdatableColumns.size () - 1) columnList.append (", ");
			}
			sql.append (columnList);
			sql.append (" ON " + tableName);
			sql.append (" BEGIN SELECT RAISE (ABORT, 'Update of " + columnList + " in " + tableName 
				+ " is forbidden.') WHERE " + triggerOverrideClause + "; END");
			
			return sql;
		}
		


		std::vector<std::string> 
		SQLiteTableSync::GetTableColumnsDb (const synthese::db::SQLiteQueueThreadExec* sqlite,
							const std::string& tableName)
		{
			std::vector<std::string> cols;
			std::string sql = "PRAGMA TABLE_INFO (" + tableName + ")";
			SQLiteResult result = sqlite->execQuery (sql);
			for (int i=0; i<result.getNbRows (); ++i)
			{
			// Column 1 is column name.
			cols.push_back (result.getColumn (i, 1));
			}
			return cols;
		}
		




		std::string 
		SQLiteTableSync::GetSQLSchemaDb (const synthese::db::SQLiteQueueThreadExec* sqlite,
						 const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='table' and name='" +
			tableName + "'";
			SQLiteResult result = sqlite->execQuery (sql);
			if (result.getNbRows () == 0) return "";
			return result.getColumn (0, 0);
		}


		
		

		std::string 
		SQLiteTableSync::GetTriggerNoInsertDb (const synthese::db::SQLiteQueueThreadExec* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_insert" + "'";
			SQLiteResult result = sqlite->execQuery (sql);
			if (result.getNbRows () == 0) return "";
			return result.getColumn (0, 0);
		}

		
		std::string 
		SQLiteTableSync::GetTriggerNoRemoveDb (const synthese::db::SQLiteQueueThreadExec* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_remove" + "'";
			SQLiteResult result = sqlite->execQuery (sql);
			if (result.getNbRows () == 0) return "";
			return result.getColumn (0, 0);
		}
		

		std::string 
		SQLiteTableSync::GetTriggerNoUpdateDb (const synthese::db::SQLiteQueueThreadExec* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_update" + "'";
			SQLiteResult result = sqlite->execQuery (sql);
			if (result.getNbRows () == 0) return "";
			return result.getColumn (0, 0);
		}


				






		
		void 
		SQLiteTableSync::adaptTable (const synthese::db::SQLiteQueueThreadExec* sqlite,
						 const std::string& tableSchema,
						 const std::string& triggerNoInsert,
						 const std::string& triggerNoRemove,
						 const std::string& triggerNoUpdate)
		{
		    
			std::string schemaDb = GetSQLSchemaDb (sqlite, getTableName ()); 
			std::string triggerNoInsertDb = GetTriggerNoInsertDb (sqlite, getTableName ());
			std::string triggerNoRemoveDb = GetTriggerNoRemoveDb (sqlite, getTableName ());
			std::string triggerNoUpdateDb = GetTriggerNoUpdateDb (sqlite, getTableName ());
		    
			if ((schemaDb == tableSchema) && 
			(triggerNoInsertDb == triggerNoInsert) &&
			(triggerNoRemoveDb == triggerNoRemove) &&
			(triggerNoUpdateDb == triggerNoUpdate)) return;
		    
		    
			std::vector<std::string> dbCols = SQLiteTableSync::GetTableColumnsDb (sqlite, _tableName);

			// Filter columns that are not in new table format
			std::stringstream colsStr;
			std::stringstream filteredColsStr;
			for (int i=0; i<dbCols.size (); ++i)
			{
				std::string dbCol (dbCols.at (i));

				if (colsStr.str ().length () > 0) colsStr << ",";
				colsStr << dbCol;

				if (_tableFormat.hasTableColumn (dbCol))
				{
					if (filteredColsStr.str ().length () > 0) filteredColsStr << ",";
					filteredColsStr << dbCol;
				}
			}
		    
			std::string buTableName = _tableName + "_backup";
			std::stringstream str;
			str << "BEGIN TRANSACTION; ";

			// Drop triggers
			if (triggerNoInsertDb != "") str << "DROP TRIGGER " << _tableName << "_no_insert ;";
			if (triggerNoRemoveDb != "") str << "DROP TRIGGER " << _tableName << "_no_remove ;";
			if (triggerNoUpdateDb != "") str << "DROP TRIGGER " << _tableName << "_no_update ;";

			// Convert table schema (through temporary table)
			str << "CREATE TEMPORARY TABLE " << buTableName << " (" 
			<< colsStr.str () << "); ";
			str << "INSERT INTO " << buTableName 
			<< " SELECT " << colsStr.str () << " FROM " << _tableName << "; ";
			str << "DROP TABLE " << _tableName << "; ";
			str << tableSchema << "; ";
			str << "INSERT INTO " << _tableName << " (" << filteredColsStr.str () << ")"
			<< " SELECT " << filteredColsStr.str () << " FROM " << buTableName << "; ";
			str << "DROP TABLE " << buTableName << "; ";

			// Redefine triggers
			if (triggerNoInsert != "") str << triggerNoInsert << " ;";
			if (triggerNoRemove != "") str << triggerNoRemove << " ;";
			if (triggerNoUpdate != "") str << triggerNoUpdate << " ;";
		    
			str << "COMMIT;";
		    
			sqlite->execUpdate (str.str ());

		}

		void SQLiteTableSync::addTableIndex( const std::vector<std::string>& columns, std::string name /*= ""*/ )
		{
			_tableFormat.addTableIndex(columns, name);
		}

		void SQLiteTableSync::addTableIndex( const std::string& column, std::string name /*= ""*/ )
		{
			vector<string> v;
			v.push_back(column);
			addTableIndex(v, name.empty() ? column : name);
		}

		std::string SQLiteTableSync::CreateIndexSQLSchema( const std::string& tableName, const SQLiteTableIndexFormat& format)
		{
			// Creation of the statement
			stringstream s;
			s	<< "CREATE INDEX " << getIndexDBName(tableName, format.first)
				<< " ON " << tableName << "(";
			for (vector<string>::const_iterator it = format.second.begin(); it != format.second.end(); ++it)
			{
				if (it != format.second.begin())
					s << ",";
				s << *it;
			}
			s << ")";

			return s.str();
		}

		std::string SQLiteTableSync::getIndexDBName( const std::string& tableName, const std::string& name )
		{
			stringstream s;
			s << tableName << "_" << name;
			return s.str();

		}

    }
}

