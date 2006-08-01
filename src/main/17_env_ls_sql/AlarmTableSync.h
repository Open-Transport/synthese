#ifndef SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Alarm;
}


namespace envlssql
{


/** 

Alarm SQLite table synchronizer.

@ingroup m17
*/

class AlarmTableSync : public ComponentTableSync
{
 private:

 public:

    AlarmTableSync (synthese::env::Environment::Registry& environments,
		    const std::string& triggerOverrideClause = "1");
    ~AlarmTableSync ();

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
