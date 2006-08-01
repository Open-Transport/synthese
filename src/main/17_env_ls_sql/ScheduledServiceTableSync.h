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
}


namespace envlssql
{


/** 

ScheduledService SQLite table synchronizer.

@ingroup m17
*/

class ScheduledServiceTableSync : public ComponentTableSync
{
 private:

 public:

    ScheduledServiceTableSync (synthese::env::Environment::Registry& environments,
			       const std::string& triggerOverrideClause = "1");
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




}

}
#endif
