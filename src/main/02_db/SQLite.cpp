#include "02_db/SQLite.h"

#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/tokenizer.hpp>

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
	    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	    boost::char_separator<char> sep (" ;\n\t");
	    tokenizer keyTokens (sql, sep);
	    
	    for (tokenizer::iterator tok_iter = keyTokens.begin(); 
		 tok_iter != keyTokens.end (); 
		 ++tok_iter) 
	    {
		std::string tok (boost::algorithm::to_upper_copy (*tok_iter));
		if (tok == "DROP") return true;
		if (tok == "DELETE") return true;
		if (tok == "UPDATE") return true;
		if (tok == "INSERT") return true;
		if (tok == "REPLACE") return true;
		if (tok == "CREATE") return true;
		if (tok == "ALTER") return true;
		std::cerr << "'" << tok << "'" << std::endl;
	    }
	    return false;
	    
	}


    }
}

