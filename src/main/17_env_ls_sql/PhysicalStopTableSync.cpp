#include "PhysicalStopTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/PhysicalStop.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



PhysicalStopTableSync::PhysicalStopTableSync (Environment::Registry& environments)
: ComponentTableSync (PHYSICALSTOPS_TABLE_NAME, environments)
{
    addTableColumn (PHYSICALSTOPS_TABLE_COL_NAME, "TEXT");
    addTableColumn (PHYSICALSTOPS_TABLE_COL_CONNECTIONPLACEID, "INTEGER");
    addTableColumn (PHYSICALSTOPS_TABLE_COL_RANKINCONNECTIONPLACE, "INTEGER");
    addTableColumn (PHYSICALSTOPS_TABLE_COL_X, "DOUBLE");
    addTableColumn (PHYSICALSTOPS_TABLE_COL_Y, "DOUBLE");
}



PhysicalStopTableSync::~PhysicalStopTableSync ()
{

}

    


void 
PhysicalStopTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    environment.getPhysicalStops ().add (createFromRow (environment, rows, rowIndex), false);
}



void 
PhysicalStopTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    synthese::env::PhysicalStop* newPhysicalStop = createFromRow (environment, rows, rowIndex);
    
    // Overwrite the old object with new object values
    *(environment.getPhysicalStops ().get (newPhysicalStop->getKey ())) = *newPhysicalStop;

    delete newPhysicalStop;
}



void 
PhysicalStopTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    environment.getPhysicalStops ().remove (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
}




synthese::env::PhysicalStop* 
PhysicalStopTableSync::createFromRow (const Environment& env,
				 const synthese::db::SQLiteResult& rows, int rowIndex) const
{
    return new synthese::env::PhysicalStop (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_NAME),
	env.getConnectionPlaces ().get (Conversion::ToInt (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_CONNECTIONPLACEID))),
	Conversion::ToInt (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_RANKINCONNECTIONPLACE)),
	Conversion::ToDouble (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_X)),
	Conversion::ToDouble (rows.getColumn (rowIndex, PHYSICALSTOPS_TABLE_COL_Y))
	);
    
}











}

}
