#ifndef SYNTHESE_ENVLSSQL_LINESTOPTABLESYNC_H
#define SYNTHESE_ENVLSSQL_LINESTOPTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class LineStop;

/** LineStop SQLite table synchronizer.
	@ingroup m15
*/
class LineStopTableSync : public ComponentTableSync
{
 public:

    LineStopTableSync ();
    ~LineStopTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};


/** Line stops table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string LINESTOPS_TABLE_NAME ("t010_line_stops");
static const std::string LINESTOPS_TABLE_COL_PHYSICALSTOPID ("physical_stop_id");
static const std::string LINESTOPS_TABLE_COL_LINEID ("line_id");
static const std::string LINESTOPS_TABLE_COL_RANKINPATH ("rank_in_path");
static const std::string LINESTOPS_TABLE_COL_ISDEPARTURE ("is_departure");
static const std::string LINESTOPS_TABLE_COL_ISARRIVAL ("is_arrival");
static const std::string LINESTOPS_TABLE_COL_METRICOFFSET ("metric_offset");
static const std::string LINESTOPS_TABLE_COL_SCHEDULEINPUT ("schedule_input");
static const std::string LINESTOPS_TABLE_COL_VIAPOINTS ("via_points");




}

}
#endif
