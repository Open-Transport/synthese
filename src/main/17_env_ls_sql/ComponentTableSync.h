#ifndef SYNTHESE_ENVLSSQL_COMPONENTTABLESYNC_H
#define SYNTHESE_ENVLSSQL_COMPONENTTABLESYNC_H


#include "module.h"


#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteResult.h"
#include "15_env/Environment.h"



namespace synthese
{
namespace envlssql
{


/** 

Component SQLite table synchronizer.
Base class for any synchronizer targetting a component which is owned by an environment.

@ingroup m17
*/

class ComponentTableSync : public synthese::db::SQLiteTableSync
{
 private:
    
    ComponentClass _componentClass;
    synthese::env::Environment::Registry& _environments;

 public:

    ComponentTableSync (const ComponentClass& componentClass,
			const std::string& tableName, 
			synthese::env::Environment::Registry& environments);

    ~ComponentTableSync ();

    void rowsAdded (const synthese::db::SQLiteThreadExec* sqlite, 
		    synthese::db::SQLiteSync* sync,
		    const synthese::db::SQLiteResult& rows);

    void rowsUpdated (const synthese::db::SQLiteThreadExec* sqlite, 
		       synthese::db::SQLiteSync* sync,
		       const synthese::db::SQLiteResult& rows);

    void rowsRemoved (const synthese::db::SQLiteThreadExec* sqlite, 
		      synthese::db::SQLiteSync* sync,
		      const synthese::db::SQLiteResult& rows);

    virtual void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
			synthese::env::Environment& target) = 0;

    virtual void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			    synthese::env::Environment& target) = 0;

    virtual void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			   synthese::env::Environment& target) = 0;

 private:

    std::vector<std::string>
	getEnvironmentsOwning (const synthese::db::SQLiteThreadExec* sqlite, 
			       const std::string& id);

};




}

}
#endif
