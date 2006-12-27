#include "AlarmTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "04_time/DateTime.h"



#include "15_env/Alarm.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::time::DateTime;

namespace synthese
{
	using namespace db;

namespace env
{



AlarmTableSync::AlarmTableSync ()
: ComponentTableSync (ALARMS_TABLE_NAME, true, true, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (ALARMS_TABLE_COL_MESSAGE, "TEXT", true);
    addTableColumn (ALARMS_TABLE_COL_PERIODSTART, "TIMESTAMP", true);
    addTableColumn (ALARMS_TABLE_COL_PERIODEND, "TIMESTAMP", true);
    addTableColumn (ALARMS_TABLE_COL_LEVEL, "INTEGER", true);
}



AlarmTableSync::~AlarmTableSync ()
{

}

    


void 
AlarmTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{

    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    if (environment.getAlarms ().contains (id)) return;

    synthese::env::Alarm* alarm = new synthese::env::Alarm (
	id,
	rows.getColumn (rowIndex, ALARMS_TABLE_COL_MESSAGE),
	DateTime::FromSQLTimestamp (rows.getColumn (rowIndex, ALARMS_TABLE_COL_PERIODSTART)),
	DateTime::FromSQLTimestamp (rows.getColumn (rowIndex, ALARMS_TABLE_COL_PERIODEND)),
	(Alarm::AlarmLevel) Conversion::ToInt (rows.getColumn (rowIndex, ALARMS_TABLE_COL_LEVEL)) );
    
    environment.getAlarms ().add (alarm);
}



void 
AlarmTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    synthese::env::Alarm* alarm = environment.getAlarms ().get (id);
    alarm->setMessage (rows.getColumn (rowIndex, ALARMS_TABLE_COL_MESSAGE));
    alarm->setPeriodStart (DateTime::FromSQLTimestamp (rows.getColumn (rowIndex, ALARMS_TABLE_COL_PERIODSTART)));
    alarm->setPeriodEnd  (DateTime::FromSQLTimestamp (rows.getColumn (rowIndex, ALARMS_TABLE_COL_PERIODEND)));
    alarm->setLevel ((Alarm::AlarmLevel) Conversion::ToInt (rows.getColumn (rowIndex, ALARMS_TABLE_COL_LEVEL)));
    
}



void 
AlarmTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getAlarms ().remove (id);
}













}

}

