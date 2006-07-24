#include "AddressTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Address.h"

#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



AddressTableSync::AddressTableSync (Environment::Registry& environments)
: ComponentTableSync (ADDRESSES_TABLE_NAME, environments, true, false)
{
    addTableColumn (ADDRESSES_TABLE_COL_CONNECTIONPLACEID, "INTEGER", false);
    addTableColumn (ADDRESSES_TABLE_COL_RANKINCONNECTIONPLACE, "INTEGER", false);
    addTableColumn (ADDRESSES_TABLE_COL_ROADID, "INTEGER", false);
    addTableColumn (ADDRESSES_TABLE_COL_METRICOFFSET, "DOUBLE", false);
    addTableColumn (ADDRESSES_TABLE_COL_X, "DOUBLE", true);
    addTableColumn (ADDRESSES_TABLE_COL_Y, "DOUBLE", true);

}



AddressTableSync::~AddressTableSync ()
{

}

    


void 
AddressTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    synthese::env::Address* address = new synthese::env::Address (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	environment.getConnectionPlaces ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_CONNECTIONPLACEID))),
	Conversion::ToInt (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_RANKINCONNECTIONPLACE)),
	environment.getRoads ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_ROADID))),
	Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_METRICOFFSET)),
	Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_X)),
	Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_Y))
	);
    

    environment.getAddresses ().add (address, false);
}



void 
AddressTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    synthese::env::Address* address = environment.getAddresses ().get (id);
    address->setX (Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_X)));
    address->setY (Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_Y)));
}



void 
AddressTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    assert (false);
}













}

}
