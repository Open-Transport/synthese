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

/** PhysicalStop SQLite table synchronizer.
	@ingroup m15
*/
class PhysicalStopTableSync : public ComponentTableSync
{
 public:

    PhysicalStopTableSync ();
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



/** Physical stops table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string PHYSICALSTOPS_TABLE_NAME ("t012_physical_stops");
static const std::string PHYSICALSTOPS_TABLE_COL_NAME ("name");
static const std::string PHYSICALSTOPS_TABLE_COL_PLACEID ("place_id");
static const std::string PHYSICALSTOPS_TABLE_COL_X ("x");
static const std::string PHYSICALSTOPS_TABLE_COL_Y ("y");


}

}
#endif

