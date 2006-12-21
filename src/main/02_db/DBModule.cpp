#include "02_db/DBModule.h"

#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteThreadExec.h"

#include "01_util/Log.h"
#include "01_util/Thread.h"

#include <iostream>

using namespace synthese::util;


namespace synthese
{
	namespace db
	{


	    SQLiteThreadExec* DBModule::_sqliteThreadExec = 0;


	    void 
	    DBModule::initialize ()
	    {

		// Initialize permanent ram loaded data
		Log::GetInstance().info("Loading live data...");
		_sqliteThreadExec = new SQLiteThreadExec (_databasePath);

		Thread sqliteThread (_sqliteThreadExec, "sqlite");
		sqliteThread.start ();
		SQLiteSync* syncHook = new SQLiteSync ();

		// Register all table syncs
		for (Factory<SQLiteTableSync>::Iterator it = 
			 Factory<SQLiteTableSync>::begin(); 
		     it != Factory<SQLiteTableSync>::end(); 
		     ++it)
		{
		    syncHook->addTableSynchronizer(it.getKey(), it.getObject());
		}
			
		_sqliteThreadExec->registerUpdateHook (syncHook);
		sqliteThread.waitForReadyState ();
			
	    }


	    SQLiteThreadExec*
	    DBModule::GetSQLite ()
	    {
		return _sqliteThreadExec;
	    }

	}
}


