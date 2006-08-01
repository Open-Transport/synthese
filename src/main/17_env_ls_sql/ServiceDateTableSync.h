#ifndef SYNTHESE_ENVLSSQL_SERVICEDATETABLESYNC_H
#define SYNTHESE_ENVLSSQL_SERVICEDATETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{

}


namespace envlssql
{


/** 

ServiceDate SQLite table synchronizer.

@ingroup m17
*/

class ServiceDateTableSync : public ComponentTableSync
{
 private:

 public:

    ServiceDateTableSync (synthese::env::Environment::Registry& environments,
			  const std::string& triggerOverrideClause = "1");
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




}

}
#endif
