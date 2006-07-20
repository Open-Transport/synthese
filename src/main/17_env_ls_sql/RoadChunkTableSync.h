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
}


namespace envlssql
{


/** 

RoadChunk SQLite table synchronizer.

@ingroup m17
*/

class RoadChunkTableSync : public ComponentTableSync
{
 private:

 public:

    RoadChunkTableSync (synthese::env::Environment::Registry& environments);
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




}

}
#endif
