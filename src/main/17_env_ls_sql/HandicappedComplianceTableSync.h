#ifndef SYNTHESE_ENVLSSQL_HANDICAPPEDCOMPLIANCETABLESYNC_H
#define SYNTHESE_ENVLSSQL_HANDICAPPEDCOMPLIANCETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
    class HandicappedCompliance;
}


namespace envlssql
{


/** 

HandicappedCompliance SQLite table synchronizer.

@ingroup m17
*/

class HandicappedComplianceTableSync : public ComponentTableSync
{
 private:

 public:

    HandicappedComplianceTableSync (synthese::env::Environment::Registry& environments,
				    const std::string& triggerOverrideClause = "1");
    ~HandicappedComplianceTableSync ();

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
