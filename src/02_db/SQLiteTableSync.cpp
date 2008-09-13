
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


#include "02_db/Constants.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteException.h"
#include "01_util/Conversion.h"
#include <sstream>

#include <boost/thread/mutex.hpp>

using namespace std;

namespace synthese
{
    using namespace util;

    namespace db
    {
	//		std::map<int, int> SQLiteTableSync::_autoIncrementValues;

		SQLiteTableSync::SQLiteTableSync (const Args& args)
		    : _allowInsert (args.allowInsert)
		    , _allowRemove (args.allowRemove)
		    , _triggerOverrideClause (args.triggerOverrideClause)
		    , _ignoreCallbacksOnFirstSync (args.ignoreCallbacksOnFirstSync)
		    , _enableTriggers (args.enableTriggers)
		    , _selectOnCallbackColumns ()
		{ }



		SQLiteTableSync::~SQLiteTableSync ()
		{

		}



	        const std::string& 
		SQLiteTableSync::getPrimaryKey () const
		{
		    if (_tableFormat.getTableColumnCount () == 0)
		    {
			throw SQLiteException ("No column defined for table " + getTableName ());
		    }
		    return _tableFormat.getTableColumn (0).name;
		}



	
	        std::string 
		SQLiteTableSync::getSelectOnCallbackColumnsClause () const
		{
		    // At least primary key!
		    if (_selectOnCallbackColumns.empty ()) return getPrimaryKey ();

		    std::stringstream ss;
		    for (int i=0; i<_selectOnCallbackColumns.size (); ++i)
		    {
			if (i != 0) ss << ",";
			ss << _selectOnCallbackColumns.at (i);
		    }
		    return ss.str ();
		}



		void 
		SQLiteTableSync::updateSchema (synthese::db::SQLite* sqlite)
		{
		    // Check if the table already exists
		    std::string sql = "SELECT * FROM SQLITE_MASTER WHERE name='" + getTableName () + "' AND type='table'";
		    
		    SQLiteResultSPtr res = sqlite->execQuery (sql);
		    std::string tableSchema = CreateSQLSchema (getTableName (), _tableFormat);
		    
		    std::string triggerNoInsert = _allowInsert ? "" : 
			CreateTriggerNoInsert (getTableName (), getTriggerOverrideClause ());
		    
		    std::string triggerNoRemove = _allowRemove ? "" : 
			CreateTriggerNoRemove (getTableName (), getTriggerOverrideClause ());
		    
		    std::string triggerNoUpdate = (_tableFormat.hasNonUpdatableColumn () == false) ? "" : 
			CreateTriggerNoUpdate (getTableName (), _tableFormat, getTriggerOverrideClause ());
		    
		    
		    if (res->next () == false)
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

		    // Indexes
		    SQLiteTableIndexMap indexes = _tableFormat.getTableIndexes();
		    for (SQLiteTableIndexMap::const_iterator it = indexes.begin(); it != indexes.end(); ++it)
		    {
			// Search if the index exists
			stringstream sql;
			sql << "SELECT sql FROM SQLITE_MASTER WHERE name='" << getIndexDBName(getTableName (), it->first)
			    << "' AND type='index'";
			
			SQLiteResultSPtr res = sqlite->execQuery (sql.str());
			
			// The index already exists
			if (res->next ())
			{
			    // The index already exists and is identical : do nothing
			    if (res->getText("sql") == CreateIndexSQLSchema(getTableName (), *it)) continue;
			    
			    // Drop the old index
			    stringstream drop;
			    drop << "DROP INDEX " << getIndexDBName(getTableName (), it->first);
			    sqlite->execUpdate(drop.str());
			}
			
			// Creation of the index
			sqlite->execUpdate(CreateIndexSQLSchema(getTableName (), *it));
		    }
		}



		void 
		SQLiteTableSync::firstSync (synthese::db::SQLite* sqlite, 
						synthese::db::SQLiteSync* sync)
		{
			// Pre-init phase
			beforeFirstSync (sqlite, sync);
			
			// Callbacks according to what already exists in the table.
			if (_ignoreCallbacksOnFirstSync == false)
			{
			    std::stringstream ss;
			    ss << "SELECT " << getSelectOnCallbackColumnsClause () << " FROM " << getTableName ();
			    SQLiteResultSPtr result = sqlite->execQuery (ss.str ());
			    rowsAdded (sqlite, sync, result, true);
			}

			// Post-init phase
			afterFirstSync (sqlite, sync);
		}

			    

		void 
		SQLiteTableSync::beforeFirstSync (synthese::db::SQLite* sqlite, 
						  synthese::db::SQLiteSync* sync)
		{
		}

		void 
		SQLiteTableSync::afterFirstSync (synthese::db::SQLite* sqlite, 
						 synthese::db::SQLiteSync* sync)
		{
		}



		int 
		SQLiteTableSync::getTableId () const
		{
			return ParseTableId (getTableName ());
		}






		const SQLiteTableFormat& 
		SQLiteTableSync::getTableFormat () const
		{
			return _tableFormat;
		}





		void 
		SQLiteTableSync::addTableColumn (const std::string& columnName, 
						 const std::string& columnType,
						 bool updatable,
						 bool selectOnCallback)
		{
		    _tableFormat.addTableColumn (columnName, columnType, updatable);
		    if (selectOnCallback) _selectOnCallbackColumns.push_back (columnName);
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
		SQLiteTableSync::createTable (synthese::db::SQLite* sqlite,
						  const std::string& tableSchema,
						  const std::string& triggerNoInsert,
						  const std::string& triggerNoRemove,
						  const std::string& triggerNoUpdate)
		{
			const SQLiteTableFormat& format = getTableFormat ();

			// Create the table if it does not already exist.
			sqlite->execUpdate (tableSchema);

			// Insert some triggers to prevent unallowed insert/update/remove operations
			if (triggerNoInsert != "") sqlite->execUpdate (triggerNoInsert);
			if (triggerNoRemove != "") sqlite->execUpdate (triggerNoRemove);
			if (triggerNoUpdate != "") sqlite->execUpdate (triggerNoUpdate);
				
		}




		std::string 
		SQLiteTableSync::CreateSQLSchema (const std::string& tableName,
						  const SQLiteTableFormat& format)
		{
			std::string sql = "CREATE TABLE " + tableName + " (";
			sql.append (format.getTableColumn(0).name).append (" ")
			.append (format.getTableColumn(0).type).append (" UNIQUE PRIMARY KEY ON CONFLICT ROLLBACK");

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
		SQLiteTableSync::GetTableColumnsDb (synthese::db::SQLite* sqlite,
							const std::string& tableName)
		{
			std::vector<std::string> cols;
			std::string sql = "PRAGMA TABLE_INFO (" + tableName + ")";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			while (result->next ())
			{
			    cols.push_back (result->getText (1));
			}
			return cols;
		}
		




		std::string 
		SQLiteTableSync::GetSQLSchemaDb (synthese::db::SQLite* sqlite,
						 const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='table' and name='" +
			tableName + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (result->next() == false) return "";
			return result->getText (0);
		}


		
		

		std::string 
		SQLiteTableSync::GetTriggerNoInsertDb (synthese::db::SQLite* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_insert" + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (result->next () == false) return "";
			return result->getText (0);
		}

		
		std::string 
		SQLiteTableSync::GetTriggerNoRemoveDb (synthese::db::SQLite* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_remove" + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (result->next () == false) return "";
			return result->getText (0);
		}
		

		std::string 
		SQLiteTableSync::GetTriggerNoUpdateDb (synthese::db::SQLite* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_update" + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (result->next () == false) return "";
			return result->getText (0);
		}


				






		
		void 
		SQLiteTableSync::adaptTable (synthese::db::SQLite* sqlite,
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
		    
		    
			std::vector<std::string> dbCols = SQLiteTableSync::GetTableColumnsDb (sqlite, getTableName ());

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
		    
			std::string buTableName = getTableName () + "_backup";
			std::stringstream str;
			str << "BEGIN TRANSACTION; ";

			// Drop triggers
			if (triggerNoInsertDb != "") str << "DROP TRIGGER " << getTableName () << "_no_insert ;";
			if (triggerNoRemoveDb != "") str << "DROP TRIGGER " << getTableName () << "_no_remove ;";
			if (triggerNoUpdateDb != "") str << "DROP TRIGGER " << getTableName () << "_no_update ;";

			// Convert table schema (through temporary table)
			str << "CREATE TEMPORARY TABLE " << buTableName << " (" 
			<< colsStr.str () << "); ";
			str << "INSERT INTO " << buTableName 
			<< " SELECT " << colsStr.str () << " FROM " << getTableName () << "; ";
			str << "DROP TABLE " << getTableName () << "; ";
			str << tableSchema << "; ";
			str << "INSERT INTO " << getTableName () << " (" << filteredColsStr.str () << ")"
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

	
	         SQLiteResultSPtr 
		 SQLiteTableSync::getRowById (synthese::db::SQLite* sqlite, const uid& id) const
		 {
		     std::stringstream ss;
		     ss << "SELECT " << getSelectOnCallbackColumnsClause () 
			<< " FROM " << getTableName () << " WHERE ROWID=" << id << " LIMIT 1";
		     
		     return sqlite->execQuery (ss.str (), false); 
		 }


    }
}

