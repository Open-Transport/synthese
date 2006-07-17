#include "SQLiteTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteThreadExec.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;


namespace synthese
{
namespace db
{



SQLiteTableSync::SQLiteTableSync ( const std::string& tableName)
: _tableName (tableName)
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

    // Create the table if it does not already exist.
    std::string sql = "CREATE TABLE IF NOT EXISTS " + getTableName () + " (";
    sql.append (format[0].first).append (" ").append (format[0].second).append (" UNIQUE PRIMARY KEY");
    for (int i=1; i< (int) format.size (); ++i)
    {
	sql.append (", ").append (format[i].first).append (" ").append (format[i].second);
    }
    sql.append (")");
    sqlite->execQuery (sql);
    
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
    return Conversion::ToInt (getTableName ().substr (1, 4));
}



const SQLiteTableFormat& 
SQLiteTableSync::getTableFormat () const
{
    return _tableFormat;
}





void 
SQLiteTableSync::addTableColumn (const std::string& columnName, 
				 const std::string& columnType)
{
    _tableFormat.push_back (std::make_pair (columnName, columnType));
}









}

}
