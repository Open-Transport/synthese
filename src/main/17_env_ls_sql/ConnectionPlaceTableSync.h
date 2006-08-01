#ifndef SYNTHESE_ENVLSSQL_CONNECTIONPLACETABLESYNC_H
#define SYNTHESE_ENVLSSQL_CONNECTIONPLACETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class ConnectionPlace;
}


namespace envlssql
{


/** 

ConnectionPlace SQLite table synchronizer.

@ingroup m17
*/

class ConnectionPlaceTableSync : public ComponentTableSync
{
 private:

 public:

    ConnectionPlaceTableSync (synthese::env::Environment::Registry& environments,
			      const std::string& triggerOverrideClause = "1");
    ~ConnectionPlaceTableSync ();

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
