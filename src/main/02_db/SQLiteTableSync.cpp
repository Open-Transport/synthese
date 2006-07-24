#include "SQLiteTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteThreadExec.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;


namespace synthese
{
namespace db
{



SQLiteTableSync::SQLiteTableSync ( const std::string& tableName,
				   bool allowInsert, 
				   bool allowRemove )
: _tableName (tableName)
, _allowInsert (allowInsert)
, _allowRemove (allowRemove)
{

}



SQLiteTableSync::~SQLiteTableSync ()
{

}



void 
SQLiteTableSync::firstSync (const synthese::db::SQLiteThreadExec* sqlite, 
			    synthese::db::SQLiteSync* sync)
{
    const SQLiteTableFormat& format = getTableFormat ();

    // Check if the table already exists

    std::string sql = "SELECT * FROM SQLITE_MASTER WHERE name='" + getTableName () + "'";
    if (sqlite->execQuery (sql).getNbRows () == 0)
    {
	// Create the table if it does not already exist.
	sql = "CREATE TABLE " + getTableName () + " (";
	sql.append (format[0].name).append (" ").append (format[0].type).append (" UNIQUE PRIMARY KEY");
	for (int i=1; i< (int) format.size (); ++i)
	{
	    sql.append (", ").append (format[i].name).append (" ").append (format[i].type);
	}
	sql.append (")");
	sqlite->execUpdate (sql);
	
	// Insert some triggers to prevent unallowed insert/update/remove operations
	if (_allowInsert == false)
	{
	    sql = "CREATE TRIGGER " ;
	    sql.append (getTableName () + "_no_insert");
	    sql.append (" BEFORE INSERT ON " + getTableName ());
	    sql.append (" BEGIN SELECT RAISE (ABORT, 'Insertion in " + getTableName () 
			+ " is forbidden.'); END;");
	    sqlite->execUpdate (sql);
	}
	
	if (_allowRemove == false)
	{
	    sql = "CREATE TRIGGER " ;
	    sql.append (getTableName () + "_no_remove");
	    sql.append (" BEFORE DELETE ON " + getTableName ());
	    sql.append (" BEGIN SELECT RAISE (ABORT, 'Deletion in " + getTableName () 
			+ " is forbidden.'); END;");
	    sqlite->execUpdate (sql);
	}

	std::vector<std::string> nonUpdatableColumns;
	for (SQLiteTableFormat::const_iterator it = _tableFormat.begin ();
	     it != _tableFormat.end (); ++it)
	{
	    SQLiteTableColumnFormat columnFormat = *it;
	    if (columnFormat.updatable == false)
	    {
		nonUpdatableColumns.push_back (columnFormat.name);
	    }
	}
	
	if (nonUpdatableColumns.empty () == false)
	{
	    sql = "CREATE TRIGGER " ;
	    sql.append (getTableName () + "_no_update");
	    sql.append (" BEFORE UPDATE OF ");
	    std::string columnList;
	    for (int i=0; i<nonUpdatableColumns.size (); ++i)
	    {
		columnList.append (nonUpdatableColumns[i]);
		if (i != nonUpdatableColumns.size () - 1) columnList.append (", ");
	    }
	    sql.append (columnList);
	    sql.append (" ON " + getTableName ());
	    sql.append (" BEGIN SELECT RAISE (ABORT, 'Update of " + columnList + " in " + getTableName () 
			+ " is forbidden.'); END;");
	    sqlite->execUpdate (sql);
	}
	
    }
    // Callbacks according to what already exists in the table.
    SQLiteResult result = sqlite->execQuery ("SELECT * FROM " + getTableName ());
    rowsAdded (sqlite, sync, result);
    
}

    

uid 
SQLiteTableSync::encodeUId (int gridId, int gridNodeId, long objectId)
{
    return synthese::util::encodeUId (getTableId (), gridId, gridNodeId, objectId);
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
    SQLiteTableColumnFormat columnFormat;
    columnFormat.name = columnName;
    columnFormat.type = columnType;
    columnFormat.updatable = updatable;
    _tableFormat.push_back (columnFormat);
}




int 
SQLiteTableSync::ParseTableId (const std::string& tableName)
{
    return Conversion::ToInt (tableName.substr (1, 4));
}







}

}
