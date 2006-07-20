#ifndef SYNTHESE_ENVLSSQL_PUBLICPLACETABLESYNC_H
#define SYNTHESE_ENVLSSQL_PUBLICPLACETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class PublicPlace;
}


namespace envlssql
{


/** 

PublicPlace SQLite table synchronizer.

@ingroup m17
*/

class PublicPlaceTableSync : public ComponentTableSync
{
 private:

 public:

    PublicPlaceTableSync (synthese::env::Environment::Registry& environments);
    ~PublicPlaceTableSync ();

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
