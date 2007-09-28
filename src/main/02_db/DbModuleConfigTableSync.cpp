#include "DbModuleConfigTableSync.h"
#include "02_db/SQLite.h"


#include "DbModuleClass.h"
#include "Constants.h"



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
		DbModuleConfigTableSync::rowsAdded (SQLite* sqlite, 
						    SQLiteSync* sync,
						    const SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    while (rows->next ())
		    {
			DbModuleClass::SetParameter (rows->getText (COL_PARAMNAME), 
						     rows->getText (COL_PARAMVALUE));
		    }
		    
		}



		void 
		DbModuleConfigTableSync::rowsUpdated (SQLite* sqlite, 
							SQLiteSync* sync,
							const SQLiteResultSPtr& rows)
		{
		    rowsAdded (sqlite, sync, rows);
		}



		void 
		DbModuleConfigTableSync::rowsRemoved (SQLite* sqlite, 
							SQLiteSync* sync,
							const SQLiteResultSPtr& rows)
		{

		}



	}
}

