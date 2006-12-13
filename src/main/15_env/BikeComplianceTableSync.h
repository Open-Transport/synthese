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

/** BikeCompliance SQLite table synchronizer.
	@ingroup m15
*/
class BikeComplianceTableSync : public ComponentTableSync
{
 public:

    BikeComplianceTableSync ();
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


static const std::string BIKECOMPLIANCES_TABLE_NAME ("t020_bike_compliances");
static const std::string BIKECOMPLIANCES_TABLE_COL_STATUS ("status");
static const std::string BIKECOMPLIANCES_TABLE_COL_CAPACITY ("capacity");


}

}
#endif

