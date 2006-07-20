#include "PublicPlaceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/PublicPlace.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;
using synthese::env::PublicPlace;
using synthese::env::City;

namespace synthese
{
namespace envlssql
{



PublicPlaceTableSync::PublicPlaceTableSync (Environment::Registry& environments)
: ComponentTableSync (PUBLICPLACES_TABLE_NAME, environments)
{
    addTableColumn (PUBLICPLACES_TABLE_COL_NAME, "TEXT");
    addTableColumn (PUBLICPLACES_TABLE_COL_CITYID, "INTEGER");
}



PublicPlaceTableSync::~PublicPlaceTableSync ()
{

}

    


void 
PublicPlaceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    std::string name (
	rows.getColumn (rowIndex, PUBLICPLACES_TABLE_COL_NAME));
    uid cityId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, PUBLICPLACES_TABLE_COL_CITYID)));

    PublicPlace* pp = new PublicPlace (id, name, environment.getCities ().get (cityId));
    environment.getPublicPlaces ().add (pp, false);

}



void 
PublicPlaceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{

}



void 
PublicPlaceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
//    environment.getCities ().remove (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
}













}

}
