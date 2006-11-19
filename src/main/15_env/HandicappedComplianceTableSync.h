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


/** 

HandicappedCompliance SQLite table synchronizer.

@ingroup m15
*/

class HandicappedComplianceTableSync : public ComponentTableSync
{
 public:

    HandicappedComplianceTableSync ();
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


static const std::string HANDICAPPEDCOMPLIANCES_TABLE_NAME ("t019_handicapped_compliances");
static const std::string HANDICAPPEDCOMPLIANCES_TABLE_COL_STATUS ("status");
static const std::string HANDICAPPEDCOMPLIANCES_TABLE_COL_CAPACITY ("capacity");


}

}
#endif
