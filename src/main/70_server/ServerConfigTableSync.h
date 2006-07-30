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
 private:

    ServerConfig& _config;

 public:

    ServerConfigTableSync (ServerConfig& config);
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


 private:

};




}

}
#endif
