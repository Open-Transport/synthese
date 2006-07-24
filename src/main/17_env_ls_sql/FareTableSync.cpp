#include "FareTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Fare.h"

#include <sqlite/sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



FareTableSync::FareTableSync (Environment::Registry& environments)
: ComponentTableSync (FARES_TABLE_NAME, environments, true, false)
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
    synthese::env::Fare* fare = new synthese::env::Fare (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, FARES_TABLE_COL_NAME),
	(synthese::env::Fare::FareType) Conversion::ToInt (rows.getColumn (rowIndex, FARES_TABLE_COL_FARETYPE))
	);
    environment.getFares ().add (fare, false);
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
    assert (false);
}









}

}
