#ifndef SYNTHESE_ENVLSSQL_SERVICEDATETABLESYNC_H
#define SYNTHESE_ENVLSSQL_SERVICEDATETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{

/** ServiceDate SQLite table synchronizer.
	@ingroup m15
*/
class ServiceDateTableSync : public ComponentTableSync
{
 public:

    ServiceDateTableSync ();
    ~ServiceDateTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

    void updateServiceCalendar (const synthese::db::SQLiteResult& rows, int rowIndex,
				synthese::env::Environment& environment,
				bool marked);


};


static const std::string SERVICEDATES_TABLE_NAME ("t005_service_dates");
static const std::string SERVICEDATES_TABLE_COL_SERVICEID ("service_id");
static const std::string SERVICEDATES_TABLE_COL_DATE("date");




}

}
#endif

