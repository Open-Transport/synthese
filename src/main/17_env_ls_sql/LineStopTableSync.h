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
}


namespace envlssql
{


/** 

LineStop SQLite table synchronizer.

@ingroup m17
*/

class LineStopTableSync : public ComponentTableSync
{
 private:

 public:

    LineStopTableSync (synthese::env::Environment::Registry& environments,
		       const std::string& triggerOverrideClause = "1");
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




}

}
#endif
