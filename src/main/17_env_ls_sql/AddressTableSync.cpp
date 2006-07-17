#include "AddressTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Address.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



AddressTableSync::AddressTableSync (Environment::Registry& environments)
: ComponentTableSync (ADDRESSES_TABLE_NAME, environments)
{
    addTableColumn (ADDRESSES_TABLE_COL_CONNECTIONPLACEID, "INTEGER");
    addTableColumn (ADDRESSES_TABLE_COL_RANKINCONNECTIONPLACE, "INTEGER");
    addTableColumn (ADDRESSES_TABLE_COL_ROADID, "INTEGER");
    addTableColumn (ADDRESSES_TABLE_COL_METRICOFFSET, "REAL");
    addTableColumn (ADDRESSES_TABLE_COL_X, "REAL");
    addTableColumn (ADDRESSES_TABLE_COL_Y, "REAL");

}



AddressTableSync::~AddressTableSync ()
{

}

    


void 
AddressTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    environment.getAddresses ().add (createFromRow (environment, rows, rowIndex), false);
}



void 
AddressTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    synthese::env::Address* newAddress = createFromRow (environment, rows, rowIndex);
    
    // Overwrite the old object with new object values
    *(environment.getAddresses ().get (newAddress->getKey ())) = *newAddress;

    delete newAddress;
}



void 
AddressTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    environment.getAddresses ().remove (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
}




synthese::env::Address* 
AddressTableSync::createFromRow (const Environment& env,
				 const synthese::db::SQLiteResult& rows, int rowIndex) const
{
    return new synthese::env::Address (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	env.getConnectionPlaces ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_CONNECTIONPLACEID))),
	Conversion::ToInt (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_RANKINCONNECTIONPLACE)),
	env.getRoads ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_ROADID))),
	Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_METRICOFFSET)),
	Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_X)),
	Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_Y))
	);
    
}











}

}
