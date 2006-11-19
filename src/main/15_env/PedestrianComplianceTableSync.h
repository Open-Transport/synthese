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

/** PedestrianCompliance SQLite table synchronizer.
	@ingroup m15
*/

class PedestrianComplianceTableSync : public ComponentTableSync
{
 public:

    PedestrianComplianceTableSync ();
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


static const std::string PEDESTRIANCOMPLIANCES_TABLE_NAME ("t018_pedestrian_compliances");
static const std::string PEDESTRIANCOMPLIANCES_TABLE_COL_STATUS ("status");
static const std::string PEDESTRIANCOMPLIANCES_TABLE_COL_CAPACITY ("capacity");



}

}
#endif
