#ifndef SYNTHESE_ENVLSSQL_CONTINUOUSSERVICETABLESYNC_H
#define SYNTHESE_ENVLSSQL_CONTINUOUSSERVICETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class ContinuousService;
}


namespace envlssql
{


/** 

ContinuousService SQLite table synchronizer.

@ingroup m17
*/

class ContinuousServiceTableSync : public ComponentTableSync
{
 private:

 public:

    ContinuousServiceTableSync (synthese::env::Environment::Registry& environments,
				const std::string& triggerOverrideClause = "1");
    ~ContinuousServiceTableSync ();

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
