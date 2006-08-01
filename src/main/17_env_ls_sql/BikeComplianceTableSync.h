#ifndef SYNTHESE_ENVLSSQL_BIKECOMPLIANCETABLESYNC_H
#define SYNTHESE_ENVLSSQL_BIKECOMPLIANCETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
    class BikeCompliance;
}


namespace envlssql
{


/** 

BikeCompliance SQLite table synchronizer.

@ingroup m17
*/

class BikeComplianceTableSync : public ComponentTableSync
{
 private:

 public:

    BikeComplianceTableSync (synthese::env::Environment::Registry& environments,
			     const std::string& triggerOverrideClause = "1");
    ~BikeComplianceTableSync ();

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
