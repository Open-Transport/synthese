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





bool 
SQLite::IsUpdateStatement (const std::string& sql)
{
    std::string str = boost::algorithm::to_upper_copy (sql);
    // SQL is an update statement if it contains one of the following keywords :
    // UPDATE, INSERT, REPLACE, CREATE, DROP, ALTER... 
    // @todo Check missing keywords...
 
    if (str.find ("UPDATE") != std::string::npos) return true;
    if (str.find ("INSERT") != std::string::npos) return true;
    if (str.find ("REPLACE") != std::string::npos) return true;
    if (str.find ("CREATE") != std::string::npos) return true;
    if (str.find ("DROP") != std::string::npos) return true;
    if (str.find ("ALTER") != std::string::npos) return true;

    return false;

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


