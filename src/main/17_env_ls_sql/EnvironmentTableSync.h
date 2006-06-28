#ifndef SYNTHESE_ENVLSSQL_ENVIRONMENTTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ENVIRONMENTTABLESYNC_H

#include "module.h"


#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"
#include "15_env/Environment.h"



namespace synthese
{


namespace envlssql
{


/** 

@ingroup m17
*/

class EnvironmentTableSync : public synthese::db::SQLiteTableSync
{
 private:

    synthese::env::Environment::Registry& _environments;

 public:

    EnvironmentTableSync (synthese::env::Environment::Registry& environments);
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




}

}
#endif
