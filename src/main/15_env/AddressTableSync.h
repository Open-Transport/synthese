#ifndef SYNTHESE_ENVLSSQL_ADDRESSTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ADDRESSTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Address;

/** Address SQLite table synchronizer.
	@ingroup m15
*/

class AddressTableSync : public ComponentTableSync
{
 public:

    AddressTableSync ();
    ~AddressTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};


/** Addresses table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string ADDRESSES_TABLE_NAME ("t002_addresses");
static const std::string ADDRESSES_TABLE_COL_PLACEID ("place_id");  // NU
static const std::string ADDRESSES_TABLE_COL_ROADID ("road_id");  // NU
static const std::string ADDRESSES_TABLE_COL_METRICOFFSET ("metric_offset");  // U ??
static const std::string ADDRESSES_TABLE_COL_X ("x");  // U ??
static const std::string ADDRESSES_TABLE_COL_Y ("y");  // U ??


}

}
#endif
