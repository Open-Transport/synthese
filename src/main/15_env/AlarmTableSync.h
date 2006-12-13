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

/** Alarm SQLite table synchronizer.
	@ingroup m15
*/
class AlarmTableSync : public ComponentTableSync
{
 public:

    AlarmTableSync ();
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

static const std::string ALARMS_TABLE_NAME ("t003_alarms");
static const std::string ALARMS_TABLE_COL_MESSAGE ("message");  
static const std::string ALARMS_TABLE_COL_PERIODSTART ("period_start"); 
static const std::string ALARMS_TABLE_COL_PERIODEND ("period_end"); 
static const std::string ALARMS_TABLE_COL_LEVEL ("level"); 



}

}
#endif

