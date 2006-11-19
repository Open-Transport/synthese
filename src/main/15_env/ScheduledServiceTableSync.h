#ifndef SYNTHESE_ENVLSSQL_SCHEDULEDSERVICETABLESYNC_H
#define SYNTHESE_ENVLSSQL_SCHEDULEDSERVICETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class ScheduledService;

/** ScheduledService SQLite table synchronizer.
	@ingroup m15
*/
class ScheduledServiceTableSync : public ComponentTableSync
{
 public:

    ScheduledServiceTableSync ();
    ~ScheduledServiceTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};




static const std::string SCHEDULEDSERVICES_TABLE_NAME ("t016_scheduled_services");
static const std::string SCHEDULEDSERVICES_TABLE_COL_SERVICENUMBER ("service_number");
static const std::string SCHEDULEDSERVICES_TABLE_COL_SCHEDULES ("schedules");
static const std::string SCHEDULEDSERVICES_TABLE_COL_PATHID ("path_id");
static const std::string SCHEDULEDSERVICES_TABLE_COL_RANKINPATH ("rank_in_path");
static const std::string SCHEDULEDSERVICES_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
static const std::string SCHEDULEDSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
static const std::string SCHEDULEDSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
static const std::string SCHEDULEDSERVICES_TABLE_COL_RESERVATIONRULEID ("reservation_rule_id");




}

}
#endif
