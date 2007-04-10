#include "PublicPlaceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/PublicPlace.h"
#include "15_env/City.h"

#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using namespace boost;

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

    shared_ptr<City> city = environment.getCities ().getUpdateable (cityId);
    shared_ptr<PublicPlace> pp(new PublicPlace (id, name, city.get()));
    city->getPublicPlacesMatcher ().add (pp->getName (), pp.get());

    environment.getPublicPlaces ().add (pp);

}



void 
PublicPlaceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    shared_ptr<PublicPlace> pp = environment.getPublicPlaces ().getUpdateable (id);
    shared_ptr<City> city = environment.getCities ().getUpdateable (pp->getCity ()->getKey ());
    city->getPublicPlacesMatcher ().remove (pp->getName ());

    std::string name (
	rows.getColumn (rowIndex, PUBLICPLACES_TABLE_COL_NAME));
    pp->setName (name);

    city->getPublicPlacesMatcher ().add (pp->getName (), pp.get());

}



void 
PublicPlaceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    shared_ptr<const PublicPlace> pp = environment.getPublicPlaces ().get (id);
    shared_ptr<City> city = environment.getCities ().getUpdateable (pp->getCity ()->getKey ());
    city->getPublicPlacesMatcher ().remove (pp->getName ());

    environment.getPublicPlaces ().remove (id);
}













}

}

