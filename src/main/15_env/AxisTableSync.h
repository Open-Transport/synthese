#ifndef SYNTHESE_ENVLSSQL_AXISTABLESYNC_H
#define SYNTHESE_ENVLSSQL_AXISTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Axis;

/** Axis SQLite table synchronizer.
	@ingroup m15
*/
class AxisTableSync : public ComponentTableSync
{
 public:

    AxisTableSync ();
    ~AxisTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:
};




/** Axes table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string AXES_TABLE_NAME ("t004_axes");
static const std::string AXES_TABLE_COL_NAME ("name");
static const std::string AXES_TABLE_COL_FREE ("free");
static const std::string AXES_TABLE_COL_ALLOWED ("allowed");


}

}
#endif

