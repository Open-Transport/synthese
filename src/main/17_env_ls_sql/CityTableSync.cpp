#include "CityTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/City.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



CityTableSync::CityTableSync (Environment::Registry& environments)
: ComponentTableSync (CITY_CLASS, CITIES_TABLE_NAME, environments)
{
    addTableColumn (CITIES_TABLE_COL_NAME, "VARCHAR(50)");
}



CityTableSync::~CityTableSync ()
{

}

    


void 
CityTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    environment.getCities ().add (createFromRow (rows, rowIndex));
}



void 
CityTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    synthese::env::City* newCity = createFromRow (rows, rowIndex);
    
    // Overwrite the old object with new object values
    *(environment.getCities ().get (newCity->getKey ())) = *newCity;

    delete newCity;
}



void 
CityTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    environment.getCities ().remove (Conversion::ToInt (rows.getColumn (rowIndex, TABLE_COL_ID)));
}




synthese::env::City* 
CityTableSync::createFromRow (const synthese::db::SQLiteResult& rows, int rowIndex) const
{
    return new synthese::env::City (
	Conversion::ToInt (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, CITIES_TABLE_COL_NAME) );
    
}











}

}
