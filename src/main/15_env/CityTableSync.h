#ifndef SYNTHESE_ENVLSSQL_CITYTABLESYNC_H
#define SYNTHESE_ENVLSSQL_CITYTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class City;

/** City SQLite table synchronizer.
	@ingroup m15
*/
class CityTableSync : public ComponentTableSync
{
 public:

    CityTableSync ();
    ~CityTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};


/** Cities table :
- on insert : insert entry in associator
- on update : update entry in associator
- on delete : X
*/
static const std::string CITIES_TABLE_NAME ("t006_cities");
static const std::string CITIES_TABLE_COL_NAME ("name");



}

}
#endif

