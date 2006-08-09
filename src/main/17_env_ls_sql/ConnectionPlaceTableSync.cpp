#include "ConnectionPlaceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/City.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;
using synthese::env::ConnectionPlace;
using synthese::env::City;

namespace synthese
{
namespace envlssql
{



ConnectionPlaceTableSync::ConnectionPlaceTableSync (Environment::Registry& environments,
						    const std::string& triggerOverrideClause)
: ComponentTableSync (CONNECTIONPLACES_TABLE_NAME, environments, true, false, triggerOverrideClause)
{
    addTableColumn (CONNECTIONPLACES_TABLE_COL_NAME, "TEXT", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_CITYID, "INTEGER", false);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_CONNECTIONTYPE, "INTEGER", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_ISCITYMAINCONNECTION, "BOOLEAN", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_DEFAULTTRANSFERDELAY, "INTEGER", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_TRANSFERDELAYS, "TEXT", true);
    addTableColumn (CONNECTIONPLACES_TABLE_COL_ALARMID, "INTEGER", true);
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

    uid alarmId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_ALARMID)));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    City* city = environment.getCities ().get (cityId);
    ConnectionPlace* cp = new synthese::env::ConnectionPlace (
	id, name, city, connectionType, defaultTransferDelay);

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

    cp->setAlarm (environment.getAlarms ().get (alarmId));

    city->getConnectionPlacesMatcher ().add (cp->getName (), cp);
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

    uid alarmId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, CONNECTIONPLACES_TABLE_COL_ALARMID)));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    ConnectionPlace* cp = environment.getConnectionPlaces ().get (id);
    City* city = environment.getCities ().get (cp->getCity ()->getKey ());

    city->getConnectionPlacesMatcher ().remove (cp->getName ());
    
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

    cp->setAlarm (environment.getAlarms ().get (alarmId));

    city->getConnectionPlacesMatcher ().add (cp->getName (), cp);

}





void 
ConnectionPlaceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

    ConnectionPlace* cp = environment.getConnectionPlaces ().get (id);
    City* city = environment.getCities ().get (cp->getCity ()->getKey ());
    city->getConnectionPlacesMatcher ().remove (cp->getName ());

    environment.getConnectionPlaces ().remove (id);
}















}

}
