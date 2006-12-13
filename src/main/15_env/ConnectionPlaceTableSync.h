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

/** ConnectionPlace SQLite table synchronizer.
	@ingroup m15
*/
class ConnectionPlaceTableSync : public ComponentTableSync
{
 public:

    ConnectionPlaceTableSync ();
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



/** Connection places table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string CONNECTIONPLACES_TABLE_NAME ("t007_connection_places");
static const std::string CONNECTIONPLACES_TABLE_COL_NAME ("name");
static const std::string CONNECTIONPLACES_TABLE_COL_CITYID ("city_id");
static const std::string CONNECTIONPLACES_TABLE_COL_CONNECTIONTYPE ("connection_type");
static const std::string CONNECTIONPLACES_TABLE_COL_ISCITYMAINCONNECTION ("is_city_main_connection");
static const std::string CONNECTIONPLACES_TABLE_COL_DEFAULTTRANSFERDELAY ("default_transfer_delay");
static const std::string CONNECTIONPLACES_TABLE_COL_TRANSFERDELAYS ("transfer_delays");
static const std::string CONNECTIONPLACES_TABLE_COL_ALARMID ("alarm_id");



}

}
#endif

