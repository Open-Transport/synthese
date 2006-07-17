#include "FareTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Fare.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



FareTableSync::FareTableSync (Environment::Registry& environments)
: ComponentTableSync (FARES_TABLE_NAME, environments)
{
    addTableColumn (FARES_TABLE_COL_NAME, "TEXT");
    addTableColumn (FARES_TABLE_COL_FARETYPE, "INTEGER");
}



FareTableSync::~FareTableSync ()
{

}

    


void 
FareTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    environment.getFares ().add (createFromRow (environment, rows, rowIndex), false);
}



void 
FareTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    synthese::env::Fare* newFare = createFromRow (environment, rows, rowIndex);
    
    // Overwrite the old object with new object values
    *(environment.getFares ().get (newFare->getKey ())) = *newFare;

    delete newFare;
}



void 
FareTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    environment.getFares ().remove (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
}




synthese::env::Fare* 
FareTableSync::createFromRow (const Environment& env,
				 const synthese::db::SQLiteResult& rows, int rowIndex) const
{
    return new synthese::env::Fare (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, FARES_TABLE_COL_NAME),
	(synthese::env::Fare::FareType) Conversion::ToInt (rows.getColumn (rowIndex, FARES_TABLE_COL_FARETYPE))
	);
    
}











}

}
