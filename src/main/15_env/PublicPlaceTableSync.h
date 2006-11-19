#ifndef SYNTHESE_ENVLSSQL_PUBLICPLACETABLESYNC_H
#define SYNTHESE_ENVLSSQL_PUBLICPLACETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{


/** 

PublicPlace SQLite table synchronizer.

@ingroup m17
*/

class PublicPlaceTableSync : public ComponentTableSync
{
 private:

 public:

    PublicPlaceTableSync ();
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


static const std::string PUBLICPLACES_TABLE_NAME ("t013_public_places");
static const std::string PUBLICPLACES_TABLE_COL_NAME ("name");
static const std::string PUBLICPLACES_TABLE_COL_CITYID ("city_id");




}

}
#endif
