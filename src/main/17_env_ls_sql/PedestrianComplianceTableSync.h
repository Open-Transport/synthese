#ifndef SYNTHESE_ENVLSSQL_PEDESTRIANCOMPLIANCETABLESYNC_H
#define SYNTHESE_ENVLSSQL_PEDESTRIANCOMPLIANCETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
    class PedestrianCompliance;
}


namespace envlssql
{


/** 

PedestrianCompliance SQLite table synchronizer.

@ingroup m17
*/

class PedestrianComplianceTableSync : public ComponentTableSync
{
 private:

 public:

    PedestrianComplianceTableSync (synthese::env::Environment::Registry& environments,
				   const std::string& triggerOverrideClause = "1");
    ~PedestrianComplianceTableSync ();

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
