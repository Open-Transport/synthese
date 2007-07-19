#include "02_db/SQLiteHandle.h"

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
	
	

	SQLiteHandle::SQLiteHandle ()
	{
	}
	
	
	
	SQLiteHandle::~SQLiteHandle ()
	{
	}
	
	
	



	SQLiteResult 
	SQLiteHandle::execQuery (const std::string& sql)
	{
	    assert (getHandle ());

	    return SQLite::ExecQuery (getHandle (), sql);
	}


	void 
	SQLiteHandle::execUpdate (const std::string& sql, bool asynchronous)
	{
	    assert (getHandle ());
	    // asynchronous is ignored in default implementation.
	    return SQLite::ExecUpdate (getHandle (), sql);
	}



	bool 
	SQLiteHandle::isTransactionOpened ()
	{
	    assert (getHandle ());
	    return SQLite::IsTransactionOpened (getHandle ());
	}
	
	
	void 
	SQLiteHandle::beginTransaction (bool exclusive)
	{
	    assert (getHandle ());
	    return SQLite::BeginTransaction (getHandle (), exclusive);
	}
	    

	    
	void 
	SQLiteHandle::commitTransaction ()
	{
	    execUpdate ("COMMIT");
	}


	    
	SQLiteStatement 
	SQLiteHandle::prepareStatement (const std::string& sql)
	{
	    assert (getHandle ());
	    return SQLite::PrepareStatement (getHandle (), sql);
	}


	void 
	SQLiteHandle::finalizeStatement (const SQLiteStatement& statement)
	{
	    SQLite::FinalizeStatement (statement);
	}

	    
    }
}

