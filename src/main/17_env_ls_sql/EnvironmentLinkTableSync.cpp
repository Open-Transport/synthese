#include "EnvironmentLinkTableSync.h"

#include "01_util/Conversion.h"

#include <sqlite/sqlite3.h>
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "CityTableSync.h"



using synthese::util::Conversion;
using synthese::db::SQLiteResult;


namespace synthese
{
namespace envlssql
{



EnvironmentLinkTableSync::EnvironmentLinkTableSync (const std::string& linkTableName,
						    synthese::env::Environment& environment)
: synthese::db::SQLiteTableSync (linkTableName)
, _environment (environment)
{
    addTableColumn (TABLE_COL_ID, "INTEGER");
    addTableColumn (ENVLINKS_TABLE_COL_LINKCLASS, "INTEGER");
    addTableColumn (ENVLINKS_TABLE_COL_LINKID, "INTEGER");
}



EnvironmentLinkTableSync::~EnvironmentLinkTableSync ()
{

}

    
void 
EnvironmentLinkTableSync::rowsAdded (const synthese::db::SQLiteThreadExec* sqlite, 
				     synthese::db::SQLiteSync* sync,
				     const synthese::db::SQLiteResult& rows)
{
    for (int i=0; i<rows.getNbRows (); ++i)
    {
	// Update the cache
	_cache.insert (std::make_pair (rows.getColumn (i, TABLE_COL_ID), rows.getColumns (i)));

	ComponentClass componentClass = (ComponentClass) 
	    Conversion::ToInt (rows.getColumn (i, ENVLINKS_TABLE_COL_LINKCLASS ));

	std::string linkId = rows.getColumn (i, ENVLINKS_TABLE_COL_LINKID );

	// Check if the component has already been added in its corresponding table
	std::string componentTableName = getComponentTable (componentClass);
	SQLiteResult existingRow = sqlite->execQuery ("SELECT * FROM " + componentTableName + 
						      " WHERE " + TABLE_COL_ID + "=" + linkId);

	if (existingRow.getNbRows () == 1) 
	{
	    ComponentTableSync* componentTableSync = dynamic_cast<ComponentTableSync*>
		(sync->getTableSynchronizer (componentTableName));
	    componentTableSync->doAdd (existingRow, 0, _environment);
	}
    }
}



void 
EnvironmentLinkTableSync::rowsUpdated (const synthese::db::SQLiteThreadExec* sqlite, 
			     synthese::db::SQLiteSync* sync,
			     const synthese::db::SQLiteResult& rows)
{
    // Not supported right now...
    throw synthese::db::SQLiteException ("Updating an environment link is not supported right now.");
}



void 
EnvironmentLinkTableSync::rowsRemoved (const synthese::db::SQLiteThreadExec* sqlite, 
				       synthese::db::SQLiteSync* sync,
				       const synthese::db::SQLiteResult& rows)
{
    for (int i=0; i<rows.getNbRows (); ++i)
    {
	std::string id = rows.getColumn (i, TABLE_COL_ID);

	// Get back cached values for concerned rows
	std::vector<std::string> cachedValues = _cache.find (id)->second;

	ComponentClass componentClass = (ComponentClass)
	    Conversion::ToInt (cachedValues.at (1));
	
	std::string linkId = cachedValues.at (2);

	// Check if the component has already been added in its corresponding table
	std::string componentTableName = getComponentTable (componentClass);
	SQLiteResult existingRow = sqlite->execQuery ("SELECT * FROM " + componentTableName + 
						      " WHERE " + TABLE_COL_ID + "=" + linkId);

	if (existingRow.getNbRows () == 1) 
	{
	    ComponentTableSync* componentTableSync = dynamic_cast<ComponentTableSync*>
		(sync->getTableSynchronizer (componentTableName));
	    componentTableSync->doRemove (existingRow, 0, _environment);
	}
	_cache.erase (id);
    }

}
	
	


std::string 
EnvironmentLinkTableSync::getComponentTable (const ComponentClass& componentClass) const
{
    if (componentClass == AXIS_CLASS) return AXES_TABLE_NAME;
    if (componentClass == CITY_CLASS) return CITIES_TABLE_NAME;
    if (componentClass == LINE_CLASS) return LINES_TABLE_NAME;
    if (componentClass == LINESTOP_CLASS) return LINESTOPS_TABLE_NAME;
    if (componentClass == LOGICALSTOP_CLASS) return LOGICALSTOPS_TABLE_NAME;
    if (componentClass == PHYSICALSTOP_CLASS) return PHYSICALSTOPS_TABLE_NAME;
    
    // ...
    return "";
}




}

}
