#include "ConnectionPlaceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/ConnectionPlace.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;
using synthese::env::ConnectionPlace;

namespace synthese
{
namespace envlssql
{



ConnectionPlaceTableSync::ConnectionPlaceTableSync (Environment::Registry& environments,
						    const std::string& triggerOverrideClause)
: ComponentTableSync (CONNECTIONPLACES_TABLE_NAME, environments, true, false, triggerOverrideClause)
{
    addTableColumn (CONNECTIONPLACES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_CITYID, "TEXT", false);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_CONNECTIONTYPE, "TEXT", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_ISCITYMAINCONNECTION, "TEXT", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_DEFAULTTRANSFERDELAY, "TEXT", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_TRANSFERDELAYS, "TEXT", true);
}



ConnectionPlaceTableSync::~ConnectionPlaceTableSync ()
{

}

    


void 
ConnectionPlaceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    std::string name (
	rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_NAME));
    uid cityId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_CITYID)));
    ConnectionPlace::ConnectionType connectionType = (ConnectionPlace::ConnectionType)
	Conversion::ToInt (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_CONNECTIONTYPE));

    bool isCityMainConnection (
	Conversion::ToBool (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_ISCITYMAINCONNECTION)));
    int defaultTransferDelay (
	Conversion::ToInt (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_DEFAULTTRANSFERDELAY)));

    std::string transferDelaysStr (
	rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_TRANSFERDELAYS));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    ConnectionPlace* cp = new synthese::env::ConnectionPlace (
	id, name, environment.getCities ().get (cityId), connectionType, defaultTransferDelay);

    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 (":");
    tokenizer tripletTokens (transferDelaysStr, sep1);
    for (tokenizer::iterator tripletIter = tripletTokens.begin();
	 tripletIter != tripletTokens.end (); ++tripletIter)
    {
	tokenizer valueTokens (*tripletIter, sep2);
	tokenizer::iterator valueIter = valueTokens.begin();

	// (departureRank:arrivalRank:transferDelay)
	cp->addTransferDelay (Conversion::ToInt (*valueIter), 
			      Conversion::ToInt (*(++valueIter)),
			      Conversion::ToInt (*(++valueIter)));
    }

    if (isCityMainConnection)
    {
	environment.getCities ().get (cityId)->addIncludedPlace (cp);
    }

    environment.getConnectionPlaces ().add (cp, false);
}






void 
ConnectionPlaceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    std::string name (
	rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_NAME));
    uid cityId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_CITYID)));
    ConnectionPlace::ConnectionType connectionType = (ConnectionPlace::ConnectionType)
	Conversion::ToInt (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_CONNECTIONTYPE));
    bool isCityMainConnection (
	Conversion::ToBool (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_ISCITYMAINCONNECTION)));
    int defaultTransferDelay (
	Conversion::ToInt (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_DEFAULTTRANSFERDELAY)));

    std::string transferDelaysStr (
	rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_TRANSFERDELAYS));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    ConnectionPlace* cp = environment.getConnectionPlaces ().get (id);
    
    cp->setName (name);
    cp->setConnectionType (connectionType);
    
    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 (":");
    tokenizer tripletTokens (transferDelaysStr, sep1);
    for (tokenizer::iterator tripletIter = tripletTokens.begin();
	 tripletIter != tripletTokens.end (); ++tripletIter)
    {
	tokenizer valueTokens (*tripletIter, sep2);
	tokenizer::iterator valueIter = valueTokens.begin();

	// (departureRank:arrivalRank:transferDelay)
	cp->addTransferDelay (Conversion::ToInt (*valueIter), 
			      Conversion::ToInt (*(++valueIter)),
			      Conversion::ToInt (*(++valueIter)));
    }

    if (isCityMainConnection)
    {
	environment.getCities ().get (cityId)->addIncludedPlace (cp);
    }
}





void 
ConnectionPlaceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getConnectionPlaces ().remove (id);
}















}

}
