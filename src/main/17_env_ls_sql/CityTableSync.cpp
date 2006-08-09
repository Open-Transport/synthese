#include "CityTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/City.h"

#include <sqlite/sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



CityTableSync::CityTableSync (Environment::Registry& environments,
			      const std::string& triggerOverrideClause)
: ComponentTableSync (CITIES_TABLE_NAME, environments, true, false, triggerOverrideClause)
{
    addTableColumn (CITIES_TABLE_COL_NAME, "TEXT", true);
}



CityTableSync::~CityTableSync ()
{

}

    


void 
CityTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    synthese::env::City* city = new synthese::env::City (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	rows.getColumn (rowIndex, CITIES_TABLE_COL_NAME) );
    
    environment.getCities ().add (city, false);
    environment.getCitiesMatcher ().add (city->getName (), city->getKey ());
}



void 
CityTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    synthese::env::City* city = environment.getCities ().get (id);

    environment.getCitiesMatcher ().remove (city->getName ());

    city->setName (rows.getColumn (rowIndex, CITIES_TABLE_COL_NAME));

    environment.getCitiesMatcher ().add (city->getName (), city->getKey ());
}



void 
CityTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    environment.getCitiesMatcher ().remove (environment.getCities ().get (id)->getName ());

    environment.getCities ().remove (id);
}















}

}
