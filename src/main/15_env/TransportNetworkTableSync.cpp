#include "TransportNetworkTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/TransportNetwork.h"

#include <sqlite/sqlite3.h>
#include <boost/logic/tribool.hpp>
#include <assert.h>


using boost::logic::tribool;

using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



TransportNetworkTableSync::TransportNetworkTableSync ()
: ComponentTableSync (TRANSPORTNETWORKS_TABLE_NAME, true, true, db::TRIGGERS_ENABLED_CLAUSE)
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

    if (environment.getTransportNetworks ().contains (id)) return;
    
    std::string name (
	rows.getColumn (rowIndex, TRANSPORTNETWORKS_TABLE_COL_NAME));
    
    synthese::env::TransportNetwork* tn = new synthese::env::TransportNetwork (id, name);

    environment.getTransportNetworks ().add (tn);
}



void 
TransportNetworkTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    synthese::env::TransportNetwork* tn = environment.getTransportNetworks ().get (id);
    std::string name (
	rows.getColumn (rowIndex, TRANSPORTNETWORKS_TABLE_COL_NAME));
    tn->setName (name);
}



void 
TransportNetworkTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getTransportNetworks ().remove (id);
}













}

}

