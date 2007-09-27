#include "SQLite.h"

#include "02_db/SQLiteLazyResult.h"
#include "02_db/SQLiteCachedResult.h"
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


/* 
// DO NOT USE THIS : blob are not handled by sqlite3_exec

int 
sqlite_callback (void* result, int nbColumns, char** values, char** columns)
{
    SQLiteCachedResult* dbResult = (SQLiteCachedResult*) result;
    dbResult->addRow (nbColumns, values, columns);
    return 0;
}

*/



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
SQLite::IsUpdateStatement (const SQLData& sql)
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
SQLite::ExecUpdate (sqlite3* handle, const SQLData& sql)
{
    // Do a batch execution (no precompilation since it can contains more than one 
    // statement which is impossible to validate wihtout executing them one by one, given one database state)
    assert (sql.size () > 0);

    char* errMsg = 0;
    int retc = sqlite3_exec (handle, 
                             sql.c_str (), 
                             0, 
                             0, &errMsg);

    if (retc != SQLITE_OK)
    {
        std::string msg (errMsg);
        sqlite3_free (errMsg);

        throw SQLiteException ("Error executing batch update \"" + Conversion::ToTruncatedString (sql) + "\" : " + 
                               msg + " (error=" + Conversion::ToString (retc) + ")");
    }
}




SQLiteResultSPtr 
SQLite::ExecQuery (const SQLiteStatementSPtr& statement, bool lazy)
{
    // TODO : lazy results are not used right now. stepping keeps the lock 
    // external queries (from interface complain about wrong thread access
    // dbtable adaptation does not work anymore!
    // This will be fixed naturally when upgrading yo sqlite 3.5 which relaxes
    // constraints on db connection thread access.
    lazy = false;

    SQLiteResultSPtr result (new SQLiteLazyResult (statement));
    if (lazy)
    {
	return result;
    }
    else
    {
	SQLiteCachedResult* cachedResult = new SQLiteCachedResult (result);
	return SQLiteResultSPtr (cachedResult);
    }
}



/*
SQLiteResultSPtr 
SQLite::ExecQuery2 (sqlite3* handle, const std::string& sql, bool lazy)
{
    // Log::GetInstance ().debug ("Executing SQLite query " + sql);
    SQLiteCachedResult* result = new SQLiteCachedResult();
    char* errMsg = 0;
    int retc;
    try {
	retc = sqlite3_exec (handle, 
			     sql.c_str (), 
			     &sqlite_callback, 
			     result, &errMsg);
    }
    catch(...){
	delete result;
	throw SQLiteException("Unknown problem in query "+ sql);
    }
    if (retc != SQLITE_OK)
    {
	std::string msg (errMsg);
	sqlite3_free (errMsg);
	delete result;
	throw SQLiteException ("Error executing query \"" + sql + " : " + 
					msg + "\" (error=" + Conversion::ToString (retc) + ")");
    }

  // Log::GetInstance ().debug ("Query successful (" + Conversion::ToString (result.getNbRows ()) + " rows).");
    return SQLiteResultSPtr (result);
}

*/

SQLiteResultSPtr 
SQLite::ExecQuery (sqlite3* handle, const std::string& sql, bool lazy)
{
    SQLiteStatementSPtr st = 
	SQLite::CompileStatement (handle, sql);
    
    return ExecQuery (st, lazy);
  
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
    

void 
SQLite::RollbackTransaction (sqlite3* handle)
{
    ExecUpdate (handle, "ROLLBACK");
}

	


SQLiteStatementSPtr 
SQLite::CompileStatement (sqlite3* handle, const std::string& sql)
{
    return SQLiteStatementSPtr (new SQLiteStatement (handle, sql));
}







void 
SQLite::ExecUpdate (const SQLiteStatementSPtr& statement)
{
    int retc = SQLITE_ROW;
    while (retc == SQLITE_ROW)
    {
	retc = sqlite3_step (statement->getStatement ());
    }
    if (retc != SQLITE_DONE)
    {
	throw SQLiteException ("Error executing precompiled statement (error=" + Conversion::ToString (retc) + ")" + 
			       Conversion::ToTruncatedString (statement->getSQL ()));
    }

}










bool 
SQLite::IsStatementComplete (const SQLData& sql)
{
    return sqlite3_complete (sql.c_str ());
}









}
}


