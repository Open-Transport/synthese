#include "02_db/SQLite.h"

#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include <boost/algorithm/string/case_conv.hpp>

using namespace synthese::util;
using namespace std;



namespace synthese
{

    namespace db
    {
	
	
	
	
	SQLite::SQLite ()
	{
	}
	
	
	
	SQLite::~SQLite ()
	{
	}
	


	SQLiteResultSPtr 
	SQLite::execQuery (const SQLData& sql, bool lazy)
	{
	    return execQuery (compileStatement (sql), lazy);
	}






	bool 
	SQLite::isTransactionOpened ()
	{
	    char* errMsg = 0;
	    // Try to open/close a fake transaction to check...
	    // Crap but only way found to achieve this.
	    int retc = sqlite3_exec (getHandle (), 
				     "BEGIN EXCLUSIVE; END;", 
				     0, 
			     0, &errMsg);
	    return (retc == 1);
	}
	
	
	void 
	SQLite::beginTransaction (bool exclusive)
	{
	    execUpdate ("BEGIN;");
	}
	    
	    
	void 
	SQLite::commitTransaction ()
	{
	    execUpdate ("COMMIT;");
	}


	void 
	SQLite::rollbackTransaction ()
	{
	    execUpdate ("ROLLBACK;");
	}



	bool 
	SQLite::IsStatementComplete (const SQLData& sql)
	{
	    return sqlite3_complete (sql.c_str ());
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


    }
}

