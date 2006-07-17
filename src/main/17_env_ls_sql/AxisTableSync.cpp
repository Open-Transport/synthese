#include "AxisTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Axis.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



AxisTableSync::AxisTableSync (Environment::Registry& environments)
: ComponentTableSync (AXES_TABLE_NAME, environments)
{
    addTableColumn (AXES_TABLE_COL_NAME, "TEXT");
    addTableColumn (AXES_TABLE_COL_FREE, "BOOLEAN");
    addTableColumn (AXES_TABLE_COL_ALLOWED, "BOOLEAN");
}



AxisTableSync::~AxisTableSync ()
{

}

    


void 
AxisTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    environment.getAxes ().add (createFromRow (environment, rows, rowIndex), false);
}



void 
AxisTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    synthese::env::Axis* newAxis = createFromRow (environment, rows, rowIndex);
    
    // Overwrite the old object with new object values
    *(environment.getAxes ().get (newAxis->getKey ())) = *newAxis;

    delete newAxis;
}



void 
AxisTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    environment.getAxes ().remove (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
}




synthese::env::Axis* 
AxisTableSync::createFromRow (const Environment& env,
				 const synthese::db::SQLiteResult& rows, int rowIndex) const
{
    return new synthese::env::Axis (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, AXES_TABLE_COL_NAME),
	Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_FREE)),
	Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_ALLOWED))
	);
    
}











}

}
