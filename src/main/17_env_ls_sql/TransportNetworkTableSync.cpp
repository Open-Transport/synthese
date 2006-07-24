#include "TransportNetworkTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/TransportNetwork.h"

#include <sqlite/sqlite3.h>
#include <boost/logic/tribool.hpp>


using boost::logic::tribool;

using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



TransportNetworkTableSync::TransportNetworkTableSync (Environment::Registry& environments)
: ComponentTableSync (TRANSPORTNETWORKS_TABLE_NAME, environments)
{
    addTableColumn (TRANSPORTNETWORKS_TABLE_COL_NAME, "TEXT");
}



TransportNetworkTableSync::~TransportNetworkTableSync ()
{

}

    


void 
TransportNetworkTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    
    std::string name (
	rows.getColumn (rowIndex, TRANSPORTNETWORKS_TABLE_COL_NAME));
    
    synthese::env::TransportNetwork* tn = new synthese::env::TransportNetwork (id, name);

    environment.getTransportNetworks ().add (tn, false);
}



void 
TransportNetworkTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
}



void 
TransportNetworkTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
}













}

}
