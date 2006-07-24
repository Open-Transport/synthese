#include "AxisTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Axis.h"

#include <sqlite/sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



AxisTableSync::AxisTableSync (Environment::Registry& environments)
: ComponentTableSync (AXES_TABLE_NAME, environments, true, false)
{
    addTableColumn (AXES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (AXES_TABLE_COL_FREE, "BOOLEAN", true);
    addTableColumn (AXES_TABLE_COL_ALLOWED, "BOOLEAN", true);
}



AxisTableSync::~AxisTableSync ()
{

}

    


void 
AxisTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    synthese::env::Axis* axis = new synthese::env::Axis (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, AXES_TABLE_COL_NAME),
	Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_FREE)),
	Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_ALLOWED))
	);
    
    environment.getAxes ().add (axis, false);
}



void 
AxisTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    synthese::env::Axis* axis = environment.getAxes ().get (id);
    axis->setName (rows.getColumn (rowIndex, AXES_TABLE_COL_NAME));
    axis->setFree (Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_FREE)));
    axis->setAllowed (Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_ALLOWED)));

}



void 
AxisTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    assert (false);
}














}

}
