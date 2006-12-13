#ifndef SYNTHESE_SERVER_SERVERCONFIGTABLESYNC_H
#define SYNTHESE_SERVER_SERVERCONFIGTABLESYNC_H


#include <string>
#include <iostream>


#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteResult.h"



namespace synthese
{

namespace db
{
    class SQLiteThreadExec;
}



namespace server
{

    class ServerConfig;

/** 

ServerConfig SQLite table synchronizer.

@ingroup m17
*/

class ServerConfigTableSync : public db::SQLiteTableSync
{
 public:

    ServerConfigTableSync ();
    ~ServerConfigTableSync ();

 protected:

    void rowsAdded (const synthese::db::SQLiteThreadExec* sqlite, 
		    synthese::db::SQLiteSync* sync,
		    const synthese::db::SQLiteResult& rows);

    void rowsUpdated (const synthese::db::SQLiteThreadExec* sqlite, 
		      synthese::db::SQLiteSync* sync,
		      const synthese::db::SQLiteResult& rows);

    void rowsRemoved (const synthese::db::SQLiteThreadExec* sqlite, 
		      synthese::db::SQLiteSync* sync,
		      const synthese::db::SQLiteResult& rows);

};

static const std::string CONFIG_TABLE_NAME ("t999_config");
static const std::string CONFIG_TABLE_COL_PARAMNAME ("param_name");
static const std::string CONFIG_TABLE_COL_PARAMVALUE ("param_value");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_PORT ("port");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_NBTHREADS ("nb_threads");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_LOGLEVEL ("log_level");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_DATADIR ("data_dir");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_TEMPDIR ("temp_dir");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPDIR ("http_temp_dir");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPURL ("http_temp_url");
static const std::string CONFIG_TABLE_COL_PARAMVALUE_TRIGGERSENABLED ("triggers_enabled");


}

}
#endif

