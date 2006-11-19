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

/** PlaceAlias SQLite table synchronizer.
	@ingroup m15
*/
class PlaceAliasTableSync : public ComponentTableSync
{
 public:

    PlaceAliasTableSync ();
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


static const std::string PLACEALIASES_TABLE_NAME ("t011_place_aliases");
static const std::string PLACEALIASES_TABLE_COL_NAME ("name");
static const std::string PLACEALIASES_TABLE_COL_ALIASEDPLACEID ("aliased_place_id");
static const std::string PLACEALIASES_TABLE_COL_CITYID ("city_id");
static const std::string PLACEALIASES_TABLE_COL_ISCITYMAINCONNECTION ("is_city_main_connection");



}

}
#endif
