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

/** TransportNetwork SQLite table synchronizer.
	@ingroup m15
*/
class TransportNetworkTableSync : public ComponentTableSync
{
 public:

    TransportNetworkTableSync ();
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


static const std::string TRANSPORTNETWORKS_TABLE_NAME ("t022_transport_networks");
static const std::string TRANSPORTNETWORKS_TABLE_COL_NAME ("name");


}

}
#endif

