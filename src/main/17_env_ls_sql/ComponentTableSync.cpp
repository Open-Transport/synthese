#include "ComponentTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



ComponentTableSync::ComponentTableSync (const std::string& tableName, 
					Environment::Registry& environments,
					bool allowInsert,
					bool allowRemove)
: synthese::db::SQLiteTableSync (tableName, allowInsert, allowRemove)
, _environments (environments)
{
    addTableColumn (TABLE_COL_ID, "INTEGER");
}



ComponentTableSync::~ComponentTableSync ()
{

}



std::vector<std::string>
ComponentTableSync::getEnvironmentsOwning (const synthese::db::SQLiteThreadExec* sqlite, const std::string& id)
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
ComponentTableSync::rowsAdded (const synthese::db::SQLiteThreadExec* sqlite, 
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
		if (_environments.contains (envId))
		{
			doAdd (rows, i, *_environments.get (envId));
		}
	}
    }
}



void 
ComponentTableSync::rowsUpdated (const synthese::db::SQLiteThreadExec* sqlite, 
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
		if (_environments.contains (envId))
		{
			doReplace (rows, i, *_environments.get (envId));
		}
	}
    }
}


void 
ComponentTableSync::rowsRemoved (const synthese::db::SQLiteThreadExec* sqlite, 
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
		if (_environments.contains (envId))
		{
			doRemove (rows, i, *_environments.get (envId));
		}
	}
    }
}














}

}
