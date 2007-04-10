#ifndef SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H
#define SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H


#include <string>
#include <iostream>


#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteResult.h"



namespace synthese
{

namespace db
{
    class SQLiteQueueThreadExec;



/** 

DbModuleConfig SQLite table synchronizer.

@ingroup m02
*/

class DbModuleConfigTableSync : public db::SQLiteTableSync
{
 public:

    static const std::string TABLE_NAME;
    static const std::string COL_PARAMNAME;
    static const std::string COL_PARAMVALUE;


    DbModuleConfigTableSync ();
    ~DbModuleConfigTableSync ();

 protected:

    void rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
		    synthese::db::SQLiteSync* sync,
		    const synthese::db::SQLiteResult& rows, bool isFirstSync = false);

    void rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
		      synthese::db::SQLiteSync* sync,
		      const synthese::db::SQLiteResult& rows);

    void rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
		      synthese::db::SQLiteSync* sync,
		      const synthese::db::SQLiteResult& rows);

};


}

}
#endif

