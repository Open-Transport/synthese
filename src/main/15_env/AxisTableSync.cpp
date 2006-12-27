#include "AxisTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/Axis.h"

#include <sqlite/sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;
using synthese::db::SQLiteResult;

namespace synthese
{
	using namespace db;

namespace env
{



AxisTableSync::AxisTableSync ()
: ComponentTableSync (AXES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
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
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    if (environment.getAxes ().contains (id)) return;

    synthese::env::Axis* axis = new synthese::env::Axis (
	id,
	rows.getColumn (rowIndex, AXES_TABLE_COL_NAME),
	Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_FREE)),
	Conversion::ToBool (rows.getColumn (rowIndex, AXES_TABLE_COL_ALLOWED))
	);
    
    environment.getAxes ().add (axis);
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
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getAxes ().remove (id);
}














}

}

