#include "PhysicalStopTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/PhysicalStop.h"

#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



PhysicalStopTableSync::PhysicalStopTableSync (Environment::Registry& environments,
					      const std::string& triggerOverrideClause)
: ComponentTableSync (PHYSICALSTOPS_TABLE_NAME, environments, true, false, triggerOverrideClause)
{
    addTableColumn (PHYSICALSTOPS_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (PHYSICALSTOPS_TABLE_COL_CONNECTIONPLACEID, "INTEGER", false);
    addTableColumn (PHYSICALSTOPS_TABLE_COL_RANKINCONNECTIONPLACE, "INTEGER", false);
    addTableColumn (PHYSICALSTOPS_TABLE_COL_X, "DOUBLE", true);
    addTableColumn (PHYSICALSTOPS_TABLE_COL_Y, "DOUBLE", true);
}



PhysicalStopTableSync::~PhysicalStopTableSync ()
{

}

    


void 
PhysicalStopTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    synthese::env::PhysicalStop* ps = new synthese::env::PhysicalStop (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_NAME),
	environment.getConnectionPlaces ().get (Conversion::ToInt (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_CONNECTIONPLACEID))),
	Conversion::ToInt (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_RANKINCONNECTIONPLACE)),
	Conversion::ToDouble (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_X)),
	Conversion::ToDouble (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_Y))
	);

    environment.getPhysicalStops ().add (ps, false);
}



void 
PhysicalStopTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    synthese::env::PhysicalStop* ps = environment.getPhysicalStops ().get (id);
    ps->setName (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_NAME));
    ps->setX (Conversion::ToDouble (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_X)));
    ps->setY (Conversion::ToDouble (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_Y)));
    
}



void 
PhysicalStopTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getPhysicalStops ().remove (id);
}













}

}
