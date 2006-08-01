#ifndef SYNTHESE_ENVLSSQL_TRANSPORTNETWORKTABLESYNC_H
#define SYNTHESE_ENVLSSQL_TRANSPORTNETWORKTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
    class TransportNetworkTableSync;
}


namespace envlssql
{


/** 

TransportNetwork SQLite table synchronizer.

@ingroup m17
*/

class TransportNetworkTableSync : public ComponentTableSync
{
 private:

 public:

    TransportNetworkTableSync (synthese::env::Environment::Registry& environments,
			       const std::string& triggerOverrideClause = "1");
    ~TransportNetworkTableSync ();

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
