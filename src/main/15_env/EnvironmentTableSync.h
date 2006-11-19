#ifndef SYNTHESE_ENVLSSQL_ENVIRONMENTTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ENVIRONMENTTABLESYNC_H

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"


namespace synthese
{


namespace env
{


/** Environment table sync.
	@ingroup m15
*/
class EnvironmentTableSync : public synthese::db::SQLiteTableSync
{
 public:

    EnvironmentTableSync ();
    ~EnvironmentTableSync ();

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

static const std::string ENVIRONMENTS_TABLE_NAME ("t000_environments");



}

}
#endif
