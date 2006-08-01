#ifndef SYNTHESE_ENVLSSQL_PLACEALIASTABLESYNC_H
#define SYNTHESE_ENVLSSQL_PLACEALIASTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class PlaceAlias;
}


namespace envlssql
{


/** 

PlaceAlias SQLite table synchronizer.

@ingroup m17
*/

class PlaceAliasTableSync : public ComponentTableSync
{
 private:

 public:

    PlaceAliasTableSync (synthese::env::Environment::Registry& environments,
			 const std::string& triggerOverrideClause = "1");
    ~PlaceAliasTableSync ();

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
