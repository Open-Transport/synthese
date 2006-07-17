#ifndef SYNTHESE_ENVLSSQL_ENVIRONMENTLINKTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ENVIRONMENTLINKTABLESYNC_H

#include "module.h"


#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"
#include "15_env/Environment.h"



namespace synthese
{


namespace envlssql
{


    class ComponentTableSync;

/** 
    Synchronizer for environment link tables.
    This class holds a static mapping of classId <=> tableName to be maintained
    each time a new component registry is added to the environment.
    This synchronizer MUST be registered after all component synchronizers so that the mapping
    is complete.

@ingroup m17
*/

class EnvironmentLinkTableSync : public synthese::db::SQLiteTableSync
{
 private:

    synthese::env::Environment::Registry& _environments;
    std::map<int, std::string> _componentTableNames; //!< Mapping (table id, table name)

    std::map<std::string, std::vector<std::string> > _cache;


 public:

    EnvironmentLinkTableSync (const synthese::db::SQLiteSync* sync,
			      synthese::env::Environment::Registry& environments);

    ~EnvironmentLinkTableSync ();

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
