#ifndef SYNTHESE_ENVLSSQL_RESERVATIONRULETABLESYNC_H
#define SYNTHESE_ENVLSSQL_RESERVATIONRULETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class ReservationRule;


/** 

ReservationRule SQLite table synchronizer.

@ingroup m17
*/

class ReservationRuleTableSync : public ComponentTableSync
{
 public:

    ReservationRuleTableSync ();
    ~ReservationRuleTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};


static const std::string RESERVATIONRULES_TABLE_NAME ("t021_reservation_rules");
static const std::string RESERVATIONRULES_TABLE_COL_TYPE ("reservation_type");
static const std::string RESERVATIONRULES_TABLE_COL_ONLINE ("online");
static const std::string RESERVATIONRULES_TABLE_COL_ORIGINISREFERENCE ("origin_is_reference");
static const std::string RESERVATIONRULES_TABLE_COL_MINDELAYMINUTES ("min_delay_minutes");
static const std::string RESERVATIONRULES_TABLE_COL_MINDELAYDAYS ("min_delay_days");
static const std::string RESERVATIONRULES_TABLE_COL_MAXDELAYDAYS ("max_delay_days");
static const std::string RESERVATIONRULES_TABLE_COL_HOURDEADLINE ("hour_deadline");
static const std::string RESERVATIONRULES_TABLE_COL_PHONEEXCHANGENUMBER ("phone_exchange_number");
static const std::string RESERVATIONRULES_TABLE_COL_PHONEEXCHANGEOPENINGHOURS ("phone_exchange_opening_hours");
static const std::string RESERVATIONRULES_TABLE_COL_DESCRIPTION ("description");
static const std::string RESERVATIONRULES_TABLE_COL_WEBSITEURL ("web_site_url");


}

}
#endif

