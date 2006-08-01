#ifndef SYNTHESE_ENVLSSQL_ROADTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ROADTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Road;
}


namespace envlssql
{


/** 

Road SQLite table synchronizer.

@ingroup m17
*/

class RoadTableSync : public ComponentTableSync
{
 private:

 public:

    RoadTableSync (synthese::env::Environment::Registry& environments,
		   const std::string& triggerOverrideClause = "1");
    ~RoadTableSync ();

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
