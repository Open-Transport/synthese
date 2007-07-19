#include "SQLite.h"

#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include <boost/algorithm/string/case_conv.hpp>


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
SQLite::OpenHandle (const boost::filesystem::path& databaseFile)
{
    // Log::GetInstance ().info ("Connecting to SQLite db " + databaseFile.string ());
    sqlite3* db;
    int retc = sqlite3_open (databaseFile.string ().c_str (), &db);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Cannot open SQLite handle to " + 
			       databaseFile.string () + "(error=" + Conversion::ToString (retc) + ")");
    }
    // Log::GetInstance ().info ("Handle to SQLite db " + databaseFile.string () + " successful.");
    return db;
}



void 
SQLite::CloseHandle (sqlite3* handle)
{
    int retc = sqlite3_close (handle);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Cannot close SQLite handle (error=" + Conversion::ToString (retc) + ")");
    }
}





bool 
SQLite::IsUpdateStatement (const std::string& sql)
{
    std::string str = boost::algorithm::to_upper_copy (sql);
    // SQL is an update statement if it contains one of the following keywords :
    // UPDATE, INSERT, REPLACE, CREATE, DROP, ALTER... 
    // @todo Check missing keywords...
 
    if (str.find ("UPDATE ") != std::string::npos) return true;
    if (str.find ("INSERT ") != std::string::npos) return true;
    if (str.find ("REPLACE ") != std::string::npos) return true;
    if (str.find ("CREATE ") != std::string::npos) return true;
    if (str.find ("DROP ") != std::string::npos) return true;
    if (str.find ("ALTER ") != std::string::npos) return true;

    return false;

}



    


void 
SQLite::ExecUpdate (sqlite3* handle, const std::string& sql)
{
    // Log::GetInstance ().debug ("Executing SQLite updtate " + sql);
    char* errMsg = 0;
    int retc = sqlite3_exec (handle, 
			     sql.c_str (), 
			     0, 
			     0, &errMsg);
//	sqlite3_finalize();
    if (retc != SQLITE_OK)
    {
	std::string msg (errMsg);
	sqlite3_free (errMsg);
	
	throw SQLiteException ("Error executing query \"" + sql + "\" : " + 
			       msg + " (error=" + Conversion::ToString (retc) + ")");
    }
    // Log::GetInstance ().debug ("Query successful.");
}



SQLiteResult 
SQLite::ExecQuery (sqlite3* handle, const std::string& sql)
{
    // Log::GetInstance ().debug ("Executing SQLite query " + sql);
    SQLiteResult result;
    char* errMsg = 0;
    int retc;
    try {
	retc = sqlite3_exec (handle, 
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



bool
SQLite::IsTransactionOpened (sqlite3* handle)
{
    char* errMsg = 0;
    // Try to open/close a fake transaction to check...
    // Crap but only way found to achieve this.
    int retc = sqlite3_exec (handle, 
			     "BEGIN EXCLUSIVE; END;", 
			     0, 
			     0, &errMsg);
    return (retc == 1);
}



void 
SQLite::BeginTransaction (sqlite3* handle, bool exclusive)
{
    std::string sql ("BEGIN ");
    // Note : we prefer not to use the DEFERRED sqlite mode right now...  
    sql += exclusive ? "EXCLUSIVE" : "IMMEDIATE";
    ExecUpdate (handle, sql);
}
    
	    
void 
SQLite::CommitTransaction (sqlite3* handle)
{
    ExecUpdate (handle, "COMMIT");
}
    



SQLiteStatement 
SQLite::PrepareStatement (sqlite3* handle, const std::string& sql)
{
    
    sqlite3_stmt* stmt;
    int retc = sqlite3_prepare_v2 (handle, sql.c_str (), sql.length (), &stmt, 0);

    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Error compiling \"" + sql + "\" (error=" + Conversion::ToString (retc) + ")");
    }
    return stmt;
}





void 
SQLite::FinalizeStatement (const SQLiteStatement& statement)
{
    int retc = sqlite3_finalize (statement);

    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Error while finalizing statement (error=" + Conversion::ToString (retc) + ")");
    }
    
}



bool 
SQLite::IsStatementComplete (const std::string& sql)
{
    return sqlite3_complete (sql.c_str ());
}








}
}


