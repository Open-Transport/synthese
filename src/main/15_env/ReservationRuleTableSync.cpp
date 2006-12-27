#include "ReservationRuleTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "04_time/Hour.h"

#include "15_env/ReservationRule.h"

#include <sqlite/sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



ReservationRuleTableSync::ReservationRuleTableSync ()
: ComponentTableSync (RESERVATIONRULES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (RESERVATIONRULES_TABLE_COL_TYPE, "INTEGER", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_ONLINE, "BOOLEAN", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_ORIGINISREFERENCE, "BOOLEAN", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_MINDELAYMINUTES, "INTEGER", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_MINDELAYDAYS, "INTEGER", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_MAXDELAYDAYS, "INTEGER", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_HOURDEADLINE, "TIME", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_PHONEEXCHANGENUMBER, "TEXT", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_PHONEEXCHANGEOPENINGHOURS, "TEXT", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_DESCRIPTION, "TEXT", true);
    addTableColumn (RESERVATIONRULES_TABLE_COL_WEBSITEURL, "TEXT", true);

}



ReservationRuleTableSync::~ReservationRuleTableSync ()
{

}

    


void 
ReservationRuleTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    if (environment.getReservationRules ().contains (id)) return;

    ReservationRule::ReservationType type = (ReservationRule::ReservationType) Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_TYPE));

    bool online (
	Conversion::ToBool (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_ONLINE)));

    bool originIsReference (
	Conversion::ToBool (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_ORIGINISREFERENCE)));

    int minDelayMinutes = Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_MINDELAYMINUTES));
    int minDelayDays = Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_MINDELAYDAYS));
    int maxDelayDays = Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_MAXDELAYDAYS));

    synthese::time::Hour hourDeadline = synthese::time::Hour::FromSQLTime (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_HOURDEADLINE));
    
    std::string phoneExchangeNumber (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_PHONEEXCHANGENUMBER));

    std::string phoneExchangeOpeningHours (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_PHONEEXCHANGEOPENINGHOURS));

    std::string description (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_DESCRIPTION));

    std::string webSiteUrl (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_WEBSITEURL));

    synthese::env::ReservationRule* rr = new synthese::env::ReservationRule (
	id, type, online, originIsReference, minDelayMinutes, minDelayDays, maxDelayDays,
	hourDeadline, phoneExchangeNumber, phoneExchangeOpeningHours, description,
	webSiteUrl);
    
    environment.getReservationRules ().add (rr);
}



void 
ReservationRuleTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    synthese::env::ReservationRule* rr = environment.getReservationRules ().get (id);

    ReservationRule::ReservationType type = (ReservationRule::ReservationType) Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_TYPE));

    bool online (
	Conversion::ToBool (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_ONLINE)));

    bool originIsReference (
	Conversion::ToBool (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_ORIGINISREFERENCE)));

    int minDelayMinutes = Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_MINDELAYMINUTES));
    int minDelayDays = Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_MINDELAYDAYS));
    int maxDelayDays = Conversion::ToInt (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_MAXDELAYDAYS));

    synthese::time::Hour hourDeadline = synthese::time::Hour::FromSQLTime (rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_HOURDEADLINE));
    
    std::string phoneExchangeNumber (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_PHONEEXCHANGENUMBER));

    std::string phoneExchangeOpeningHours (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_PHONEEXCHANGEOPENINGHOURS));

    std::string description (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_DESCRIPTION));

    std::string webSiteUrl (
	rows.getColumn (rowIndex, RESERVATIONRULES_TABLE_COL_WEBSITEURL));

    rr->setType (type);
    rr->setOnline (online);
    rr->setOriginIsReference (originIsReference);
    rr->setMinDelayMinutes (minDelayMinutes);
    rr->setMinDelayDays (minDelayDays);
    rr->setMaxDelayDays (maxDelayDays);
    rr->setHourDeadLine (hourDeadline);
    rr->setPhoneExchangeNumber (phoneExchangeNumber);
    rr->setPhoneExchangeOpeningHours (phoneExchangeOpeningHours);
    rr->setDescription (description);
    rr->setWebSiteUrl (webSiteUrl);

}



void 
ReservationRuleTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getReservationRules ().remove (id);
}















}

}

