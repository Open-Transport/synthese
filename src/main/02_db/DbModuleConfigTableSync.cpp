#include "DbModuleConfigTableSync.h"

#include "DbModuleClass.h"




namespace synthese
{
	namespace db
	{

	    const std::string DbModuleConfigTableSync::TABLE_NAME ("t999_config");
	    const std::string DbModuleConfigTableSync::COL_PARAMNAME ("param_name");
	    const std::string DbModuleConfigTableSync::COL_PARAMVALUE ("param_value");


		DbModuleConfigTableSync::DbModuleConfigTableSync ()
		: synthese::db::SQLiteTableSync (TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (COL_PARAMNAME, "TEXT", false);
			addTableColumn (COL_PARAMVALUE, "TIMESTAMP", true);
		}



		DbModuleConfigTableSync::~DbModuleConfigTableSync ()
		{

		}

		    


		void 
		DbModuleConfigTableSync::rowsAdded (SQLiteQueueThreadExec* sqlite, 
						SQLiteSync* sync,
						const SQLiteResult& rows, bool isFirstSync)
		{
		    for (int i=0; i<rows.getNbRows (); ++i)
		    {
			DbModuleClass::SetParameter (
			    rows.getColumn (i, COL_PARAMNAME),
			    rows.getColumn (i, COL_PARAMVALUE)
			    );
		    }
		}



		void 
		DbModuleConfigTableSync::rowsUpdated (SQLiteQueueThreadExec* sqlite, 
							SQLiteSync* sync,
							const SQLiteResult& rows)
		{
		    rowsAdded (sqlite, sync, rows);
		}



		void 
		DbModuleConfigTableSync::rowsRemoved (SQLiteQueueThreadExec* sqlite, 
							SQLiteSync* sync,
							const SQLiteResult& rows)
		{

		}



	}
}

