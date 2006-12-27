#include "FareTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/Fare.h"

#include <sqlite/sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



FareTableSync::FareTableSync ()
: ComponentTableSync (FARES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (FARES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (FARES_TABLE_COL_FARETYPE, "INTEGER", true);
}



FareTableSync::~FareTableSync ()
{

}

    


void 
FareTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    
    if (environment.getFares ().contains (id)) return;

    synthese::env::Fare* fare = new synthese::env::Fare (
	id,
	rows.getColumn (rowIndex, FARES_TABLE_COL_NAME),
	(synthese::env::Fare::FareType) Conversion::ToInt (rows.getColumn (rowIndex, FARES_TABLE_COL_FARETYPE))
	);
    environment.getFares ().add (fare);
}



void 
FareTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    synthese::env::Fare* fare = environment.getFares ().get (id);
    
    fare->setName (rows.getColumn (rowIndex, FARES_TABLE_COL_NAME));
    fare->setType ((synthese::env::Fare::FareType) Conversion::ToInt (rows.getColumn (rowIndex, FARES_TABLE_COL_FARETYPE)));
    
}



void 
FareTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getFares ().remove (id);
}









}

}

