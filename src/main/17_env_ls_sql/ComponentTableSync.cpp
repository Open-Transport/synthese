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



ComponentTableSync::ComponentTableSync (const ComponentClass& componentClass,
					const std::string& tableName, 
					Environment::Registry& environments)
: synthese::db::SQLiteTableSync (tableName)
, _componentClass (componentClass)
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

    // Query for all environment
    SQLiteResult rows = sqlite->execQuery ("SELECT * FROM " + ENVIRONMENTS_TABLE_NAME);
    for (int i=0; i<rows.getNbRows (); ++i)
    {
	// Get the id and the table field
	std::string envId = rows.getColumn (i, TABLE_COL_ID);
	std::string linkTableName = rows.getColumn (i, ENVIRONMENTS_TABLE_COL_LINKTABLE);
	
	// Does the link table contains id ?
	SQLiteResult idFound = sqlite->execQuery ("SELECT " + TABLE_COL_ID + " FROM " + linkTableName 
						  + " WHERE " + ENVLINKS_TABLE_COL_LINKID + "=" + id + " AND "
						  + ENVLINKS_TABLE_COL_LINKCLASS + "=" 
						  + Conversion::ToString ((int) _componentClass));

	if (idFound.getNbRows () == 1) 
	{
	    result.push_back (envId);
	}
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
