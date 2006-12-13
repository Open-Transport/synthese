#include "SQLite.h"

#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"


using synthese::util::Conversion;
using synthese::util::Log;


namespace synthese
{
namespace db
{


int 
sqlite_callback (void* result, int nbColumns, char** values, char** columns)
{
    SQLiteResult* dbResult = (SQLiteResult*) result;
    dbResult->addRow (nbColumns, values, columns);
    return 0;
}




sqlite3* 
SQLite::OpenConnection (const boost::filesystem::path& databaseFile)
{
    // Log::GetInstance ().info ("Connecting to SQLite db " + databaseFile.string ());
    sqlite3* db;
    int retc = sqlite3_open (databaseFile.string ().c_str (), &db);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Cannot open SQLite connection to " + 
			       databaseFile.string () + "(error=" + Conversion::ToString (retc) + ")");
    }
    // Log::GetInstance ().info ("Connection to SQLite db " + databaseFile.string () + " successful.");
    return db;
}



void 
SQLite::CloseConnection (sqlite3* connection)
{
    int retc = sqlite3_close (connection);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Cannot close SQLite connection (error=" + Conversion::ToString (retc) + ")");
    }
}


    


void 
SQLite::ExecUpdate (sqlite3* connection, const std::string& sql)
{
    // Log::GetInstance ().debug ("Executing SQLite updtate " + sql);
    char* errMsg = 0;
    int retc = sqlite3_exec (connection, 
			     sql.c_str (), 
			     0, 
			     0, &errMsg);
//	sqlite3_finalize();
    if (retc != SQLITE_OK)
    {
	std::string msg (errMsg);
	sqlite3_free (errMsg);
	throw SQLiteException ("Error executing query \"" + sql + " : " + 
			       msg + "\" (error=" + Conversion::ToString (retc) + ")");
    }
    // Log::GetInstance ().debug ("Query successful.");
}



SQLiteResult 
SQLite::ExecQuery (sqlite3* connection, const std::string& sql)
{
    // Log::GetInstance ().debug ("Executing SQLite query " + sql);
    SQLiteResult result;
    char* errMsg = 0;
	int retc;
	try {
		retc = sqlite3_exec (connection, 
					sql.c_str (), 
					&sqlite_callback, 
					&result, &errMsg);
	}
	catch(...){
		throw SQLiteException("Unknown problem in query "+ sql);
	}
    if (retc != SQLITE_OK)
    {
		std::string msg (errMsg);
		sqlite3_free (errMsg);
		throw SQLiteException ("Error executing query \"" + sql + " : " + 
					msg + "\" (error=" + Conversion::ToString (retc) + ")");
    }
    // Log::GetInstance ().debug ("Query successful (" + Conversion::ToString (result.getNbRows ()) + " rows).");
    return result;
}



}
}


