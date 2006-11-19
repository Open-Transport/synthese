#ifndef SYNTHESE_ENVLSSQL_ROADCHUNKTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ROADCHUNKTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class RoadChunk;

/** RoadChunk SQLite table synchronizer.
	@ingroup m15
*/

class RoadChunkTableSync : public ComponentTableSync
{
 public:

    RoadChunkTableSync ();
    ~RoadChunkTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};



/** Road chunks table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string ROADCHUNKS_TABLE_NAME ("t014_road_chunks");
static const std::string ROADCHUNKS_TABLE_COL_ADDRESSID ("address_id");
static const std::string ROADCHUNKS_TABLE_COL_RANKINPATH ("rank_in_path");
static const std::string ROADCHUNKS_TABLE_COL_ISDEPARTURE ("is_departure");
static const std::string ROADCHUNKS_TABLE_COL_ISARRIVAL ("is_arrival");
static const std::string ROADCHUNKS_TABLE_COL_VIAPOINTS ("via_points");  // list of ids



}

}
#endif
