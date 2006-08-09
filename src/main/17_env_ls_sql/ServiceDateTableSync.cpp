#include "ServiceDateTableSync.h"

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "04_time/Date.h"

#include "15_env/Service.h"

#include <sqlite/sqlite3.h>

#include "assert.h"



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::time::Date;

using synthese::env::Environment;
using synthese::env::Service;

namespace synthese
{
namespace envlssql
{



ServiceDateTableSync::ServiceDateTableSync (Environment::Registry& environments,
					    const std::string& triggerOverrideClause)
: ComponentTableSync (SERVICEDATES_TABLE_NAME, environments, true, true, triggerOverrideClause)
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
    int tableId = synthese::util::decodeTableId (serviceId);

    Service* service = 0;
    if (tableId == ParseTableId (SCHEDULEDSERVICES_TABLE_NAME))
    {
	service = environment.getScheduledServices ().get (serviceId);
    }
    else if (tableId == ParseTableId (CONTINUOUSSERVICES_TABLE_NAME ))
    {
	service = environment.getContinuousServices ().get (serviceId);
    }
    assert (service != 0);

    // Mark the date in service calendar
    Date newDate = Date::FromSQLDate (rows.getColumn (rowIndex, SERVICEDATES_TABLE_COL_DATE));
    service->getCalendar ().mark (newDate, marked);

    environment.updateMinMaxDatesInUse (newDate, marked);
    
}
    









}

}
