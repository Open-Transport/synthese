#ifndef SYNTHESE_ENVLSSQL_FARETABLESYNC_H
#define SYNTHESE_ENVLSSQL_FARETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Fare;
}


namespace envlssql
{


/** 

Fare SQLite table synchronizer.

@ingroup m17
*/

class FareTableSync : public ComponentTableSync
{
 private:

 public:

    FareTableSync (synthese::env::Environment::Registry& environments);
    ~FareTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

    synthese::env::Fare* createFromRow (const synthese::env::Environment& env,
					const synthese::db::SQLiteResult& rows, 
					int rowIndex) const;
};




}

}
#endif
