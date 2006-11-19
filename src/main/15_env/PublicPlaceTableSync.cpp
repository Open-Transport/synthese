#include "PublicPlaceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/PublicPlace.h"
#include "15_env/City.h"

#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



PublicPlaceTableSync::PublicPlaceTableSync ()
: ComponentTableSync (PUBLICPLACES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (PUBLICPLACES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (PUBLICPLACES_TABLE_COL_CITYID, "INTEGER", false);
}



PublicPlaceTableSync::~PublicPlaceTableSync ()
{

}

    


void 
PublicPlaceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    if (environment.getPublicPlaces ().contains (id)) return;

    std::string name (
	rows.getColumn (rowIndex, PUBLICPLACES_TABLE_COL_NAME));
    uid cityId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, PUBLICPLACES_TABLE_COL_CITYID)));

    City* city = environment.getCities ().get (cityId);
    PublicPlace* pp = new PublicPlace (id, name, city);
    city->getPublicPlacesMatcher ().add (pp->getName (), pp);

    environment.getPublicPlaces ().add (pp);

}



void 
PublicPlaceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    PublicPlace* pp = environment.getPublicPlaces ().get (id);
    City* city = environment.getCities ().get (pp->getCity ()->getKey ());
    city->getPublicPlacesMatcher ().remove (pp->getName ());

    std::string name (
	rows.getColumn (rowIndex, PUBLICPLACES_TABLE_COL_NAME));
    pp->setName (name);

    city->getPublicPlacesMatcher ().add (pp->getName (), pp);

}



void 
PublicPlaceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    PublicPlace* pp = environment.getPublicPlaces ().get (id);
    City* city = environment.getCities ().get (pp->getCity ()->getKey ());
    city->getPublicPlacesMatcher ().remove (pp->getName ());

    environment.getPublicPlaces ().remove (id);
}













}

}
