
#include <sqlite/sqlite3.h>

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/EnvModule.h"
#include "15_env/EnvironmentLinkTableSync.h"
#include "15_env/ComponentTableSync.h"

using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

	namespace env
	{
		ComponentTableSync::ComponentTableSync (const std::string& tableName, 
							bool allowInsert,
							bool allowRemove,
							const std::string& triggerOverrideClause)
		: synthese::db::SQLiteTableSync (tableName, allowInsert, allowRemove, triggerOverrideClause)
		{
			addTableColumn (TABLE_COL_ID, "INTEGER");
		}



		ComponentTableSync::~ComponentTableSync ()
		{

		}



		std::vector<std::string>
		ComponentTableSync::getEnvironmentsOwning (const synthese::db::SQLiteQueueThreadExec* sqlite, const std::string& id)
		{
			std::vector<std::string> result;

			SQLiteResult rows = sqlite->execQuery ("SELECT " + ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + 
							" FROM " + ENVIRONMENT_LINKS_TABLE_NAME +
							" WHERE " + ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID + "=" + id);

			for (int i=0; i<rows.getNbRows (); ++i)
			{
				result.push_back (rows.getColumn (i, 0));
			}
		    
			return result;
		}
		    


		void 
		ComponentTableSync::rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
					synthese::db::SQLiteSync* sync,
					const synthese::db::SQLiteResult& rows)
		{
			// Look in environment link tables for each row id
			for (int i=0; i<rows.getNbRows (); ++i)
			{
			std::vector<std::string> envIds = getEnvironmentsOwning (sqlite, rows.getColumn (i, TABLE_COL_ID));
			for (std::vector<std::string>::const_iterator it = envIds.begin ();
				it != envIds.end (); ++it)
			{
				int envId = Conversion::ToInt (*it);
				if (EnvModule::getEnvironments().contains (envId))
				{
					doAdd (rows, i, * EnvModule::getEnvironments().get (envId));
				}
			}
			}
		}



		void 
		ComponentTableSync::rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						synthese::db::SQLiteSync* sync,
						const synthese::db::SQLiteResult& rows)
		{
			// Look in environment link tables for each row id
			for (int i=0; i<rows.getNbRows (); ++i)
			{
				std::vector<std::string> envIds = getEnvironmentsOwning (sqlite, rows.getColumn (i, TABLE_COL_ID));
				for (std::vector<std::string>::const_iterator it = envIds.begin ();
					it != envIds.end (); ++it)
				{
					int envId = Conversion::ToInt (*it);
					if (EnvModule::getEnvironments().contains (envId))
					{
						doReplace (rows, i, *EnvModule::getEnvironments().get (envId));
					}
				}
			}
		}


		void 
		ComponentTableSync::rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						synthese::db::SQLiteSync* sync,
						const synthese::db::SQLiteResult& rows)
		{
			// Look in environment link tables for each row id
			for (int i=0; i<rows.getNbRows (); ++i)
			{
			std::vector<std::string> envIds = getEnvironmentsOwning (sqlite, rows.getColumn (i, TABLE_COL_ID));
			for (std::vector<std::string>::const_iterator it = envIds.begin ();
				it != envIds.end (); ++it)
			{
				int envId = Conversion::ToInt (*it);
				if (EnvModule::getEnvironments().contains (envId))
				{
					doRemove (rows, i, *EnvModule::getEnvironments().get (envId));
				}
			}
			}
		}
	}
}

