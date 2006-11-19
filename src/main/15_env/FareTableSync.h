#ifndef SYNTHESE_ENVLSSQL_FARETABLESYNC_H
#define SYNTHESE_ENVLSSQL_FARETABLESYNC_H


#include <string>
#include <iostream>

#include "15_env/ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Fare;

/** Fare SQLite table synchronizer.
	@ingroup m15
*/
class FareTableSync : public ComponentTableSync
{
 public:

    FareTableSync ();
    ~FareTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};



static const std::string FARES_TABLE_NAME ("t008_fares");
static const std::string FARES_TABLE_COL_NAME ("name");
static const std::string FARES_TABLE_COL_FARETYPE ("fare_type");


}

}
#endif
