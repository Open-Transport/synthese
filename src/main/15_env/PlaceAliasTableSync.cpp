#include "PlaceAliasTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/PlaceAlias.h"
#include "15_env/City.h"

#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{


PlaceAliasTableSync::PlaceAliasTableSync ()
: ComponentTableSync (PLACEALIASES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (PLACEALIASES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (PLACEALIASES_TABLE_COL_ALIASEDPLACEID, "INTEGER", false);
    addTableColumn (PLACEALIASES_TABLE_COL_CITYID, "INTEGER", false);
    addTableColumn (PLACEALIASES_TABLE_COL_ISCITYMAINCONNECTION, "BOOLEAN", false);
}



PlaceAliasTableSync::~PlaceAliasTableSync ()
{

}

    


void 
PlaceAliasTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    if (environment.getPlaceAliases ().contains (id)) return;

    std::string name (
	rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_NAME));
    uid aliasedPlaceId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_ALIASEDPLACEID)));
    uid cityId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_CITYID)));
    int tableId = synthese::util::decodeTableId (aliasedPlaceId);

    const Place* place = environment.fetchPlace (aliasedPlaceId);

    if (place == 0) return;

    City* city = environment.getCities ().get (cityId);
    PlaceAlias* pa = new PlaceAlias (id, name, place, city);

    bool isCityMainConnection (
	Conversion::ToBool (rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_ISCITYMAINCONNECTION)));

    if (isCityMainConnection)
    {
	city->addIncludedPlace (pa);
    }

    city->getPlaceAliasesMatcher ().add (pa->getName (), pa);
    environment.getPlaceAliases ().add (pa);

}



void 
PlaceAliasTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    PlaceAlias* pa = environment.getPlaceAliases ().get (id);
    City* city = environment.getCities ().get (pa->getCity ()->getKey ());
    pa->setName (rows.getColumn (rowIndex, PLACEALIASES_TABLE_COL_NAME));
    city->getPlaceAliasesMatcher ().add (pa->getName (), pa);

}



void 
PlaceAliasTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    // TODO not finished
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    PlaceAlias* pa = environment.getPlaceAliases ().get (id);
    City* city = environment.getCities ().get (pa->getCity ()->getKey ());
    city->getPlaceAliasesMatcher ().remove (pa->getName ());
    environment.getPlaceAliases ().remove (id);
}













}

}

