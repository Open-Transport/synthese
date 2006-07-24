#include "PlaceAliasTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/PlaceAlias.h"

#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;
using synthese::env::PlaceAlias;
using synthese::env::Place;

namespace synthese
{
namespace envlssql
{



PlaceAliasTableSync::PlaceAliasTableSync (Environment::Registry& environments)
: ComponentTableSync (PLACEALIASES_TABLE_NAME, environments, true, false)
{
    addTableColumn (PLACEALIASES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (PLACEALIASES_TABLE_COL_ALIASEDPLACEID, "INTEGER", false);
}



PlaceAliasTableSync::~PlaceAliasTableSync ()
{

}

    


void 
PlaceAliasTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    std::string name (
	rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_NAME));
    uid aliasedPlaceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_ALIASEDPLACEID)));
    int tableId = synthese::util::decodeTableId (aliasedPlaceId);

    Place* place = 0;
    if (tableId == ParseTableId (PLACEALIASES_TABLE_NAME))
    {
	place = environment.getPlaceAliases ().get (aliasedPlaceId);
    }
    else if (tableId == ParseTableId (PUBLICPLACES_TABLE_NAME ))
    {
	place = environment.getPublicPlaces ().get (aliasedPlaceId);
    }
    else if (tableId == ParseTableId (CONNECTIONPLACES_TABLE_NAME ))
    {
	place = environment.getConnectionPlaces ().get (aliasedPlaceId);
    }
    if (place == 0) return;


    PlaceAlias* pa = new PlaceAlias (id, name, place);
    environment.getPlaceAliases ().add (pa, false);

}



void 
PlaceAliasTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    PlaceAlias* pa = environment.getPlaceAliases ().get (id);
    pa->setName (rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_NAME));
}



void 
PlaceAliasTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    assert (false);
}













}

}
