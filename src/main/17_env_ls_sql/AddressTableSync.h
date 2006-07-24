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
}


namespace envlssql
{


/** 

Address SQLite table synchronizer.

@ingroup m17
*/

class AddressTableSync : public ComponentTableSync
{
 private:

 public:

    AddressTableSync (synthese::env::Environment::Registry& environments);
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




}

}
#endif
