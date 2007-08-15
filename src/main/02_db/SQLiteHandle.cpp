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
	
	
	



	SQLiteResultSPtr 
	SQLiteHandle::execQuery (const SQLiteStatementSPtr& statement, bool lazy)
	{

	    return SQLite::ExecQuery (statement, lazy);
	}


	SQLiteResultSPtr 
	SQLiteHandle::execQuery (const SQLData& sql, bool lazy)
	{

	    return SQLite::ExecQuery (getHandle (), sql, lazy);
	}


	void 
	SQLiteHandle::execUpdate (const SQLiteStatementSPtr& statement, bool asynchronous)
	{

	    // asynchronous is ignored in default implementation.
	    SQLite::ExecUpdate (statement);
	}



	void 
	SQLiteHandle::execUpdate (const SQLData& sql, bool asynchronous)
	{

	    // asynchronous is ignored in default implementation.
	    SQLite::ExecUpdate (getHandle (), sql);
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
	    SQLite::CommitTransaction (getHandle ());
	}


	void 
	SQLiteHandle::rollbackTransaction ()
	{
	    SQLite::RollbackTransaction (getHandle ());
	}


	    
	SQLiteStatementSPtr 
	SQLiteHandle::compileStatement (const SQLData& sql)
	{
	    assert (getHandle ());
	    return SQLite::CompileStatement (getHandle (), sql);
	}




    }
}

