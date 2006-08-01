#ifndef SYNTHESE_ENVLSSQL_LINETABLESYNC_H
#define SYNTHESE_ENVLSSQL_LINETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
    class LineTableSync;
}


namespace envlssql
{


/** 

Line SQLite table synchronizer.

@ingroup m17
*/

class LineTableSync : public ComponentTableSync
{
 private:

 public:

    LineTableSync (synthese::env::Environment::Registry& environments,
		   const std::string& triggerOverrideClause = "1");
    ~LineTableSync ();

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
