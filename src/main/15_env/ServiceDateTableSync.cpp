#include "ServiceDateTableSync.h"

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "04_time/Date.h"

#include "15_env/Service.h"

#include <sqlite/sqlite3.h>

#include "assert.h"



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::time::Date;

namespace synthese
{
namespace env
{



ServiceDateTableSync::ServiceDateTableSync ()
: ComponentTableSync (SERVICEDATES_TABLE_NAME, true, true, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (SERVICEDATES_TABLE_COL_SERVICEID, "INTEGER", false);
    addTableColumn (SERVICEDATES_TABLE_COL_DATE , "DATE", false);
}



ServiceDateTableSync::~ServiceDateTableSync ()
{

}




void 
ServiceDateTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
			     synthese::env::Environment& environment)
{
    updateServiceCalendar (rows, rowIndex, environment, true);
}



void 
ServiceDateTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    // Cannot happen (trigger).
    assert (false);
}



void 
ServiceDateTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    updateServiceCalendar (rows, rowIndex, environment, false);
}


void 
ServiceDateTableSync::updateServiceCalendar (const synthese::db::SQLiteResult& rows, int rowIndex,
					     synthese::env::Environment& environment,
					     bool marked) 
{
    // Get the corresponding calendar
    uid serviceId = Conversion::ToLongLong (rows.getColumn (rowIndex, SERVICEDATES_TABLE_COL_SERVICEID));

    Service* service = environment.fetchService (serviceId);
    assert (service != 0);

    // Mark the date in service calendar
    Date newDate = Date::FromSQLDate (rows.getColumn (rowIndex, SERVICEDATES_TABLE_COL_DATE));
    service->getCalendar ().mark (newDate, marked);

    environment.updateMinMaxDatesInUse (newDate, marked);
    
}
    









}

}

