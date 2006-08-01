#ifndef SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H
#define SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class PhysicalStop;
}


namespace envlssql
{


/** 

PhysicalStop SQLite table synchronizer.

@ingroup m17
*/

class PhysicalStopTableSync : public ComponentTableSync
{
 private:

 public:

    PhysicalStopTableSync (synthese::env::Environment::Registry& environments,
			   const std::string& triggerOverrideClause = "1");
    ~PhysicalStopTableSync ();

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
