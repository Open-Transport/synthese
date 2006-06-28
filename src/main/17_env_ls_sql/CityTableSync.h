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
}


namespace envlssql
{


/** 

City SQLite table synchronizer.

@ingroup m17
*/

class CityTableSync : public ComponentTableSync
{
 private:

 public:

    CityTableSync (synthese::env::Environment::Registry& environments);
    ~CityTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

    synthese::env::City* createFromRow (const synthese::db::SQLiteResult& rows, 
					int rowIndex) const;
};




}

}
#endif
