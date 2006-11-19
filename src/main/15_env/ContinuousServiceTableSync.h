#ifndef SYNTHESE_ENVLSSQL_CONTINUOUSSERVICETABLESYNC_H
#define SYNTHESE_ENVLSSQL_CONTINUOUSSERVICETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class ContinuousService;

/** ContinuousService SQLite table synchronizer.
	@ingroup m15
*/
class ContinuousServiceTableSync : public ComponentTableSync
{
 public:

	 /** Continuous service SQLite table constructor.
	 */
    ContinuousServiceTableSync ();
    ~ContinuousServiceTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};



static const std::string CONTINUOUSSERVICES_TABLE_NAME ("t017_continuous_services");
static const std::string CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER ("service_number");
static const std::string CONTINUOUSSERVICES_TABLE_COL_SCHEDULES ("schedules");
static const std::string CONTINUOUSSERVICES_TABLE_COL_PATHID ("path_id");
static const std::string CONTINUOUSSERVICES_TABLE_COL_RANGE ("range");
static const std::string CONTINUOUSSERVICES_TABLE_COL_MAXWAITINGTIME ("max_waiting_time");
static const std::string CONTINUOUSSERVICES_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
static const std::string CONTINUOUSSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
static const std::string CONTINUOUSSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
//... 


}

}
#endif
