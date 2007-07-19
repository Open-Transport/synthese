#ifndef SYNTHESE_UTIL_SQLITEUPDATEHOOK_H
#define SYNTHESE_UTIL_SQLITEUPDATEHOOK_H


#include "01_util/ThreadExec.h"

#include <vector>
#include <string>
#include <deque>

#include <boost/filesystem/path.hpp>

#include <sqlite/sqlite3.h>


namespace synthese
{


namespace db
{

    class SQLiteQueueThreadExec;



    typedef 
	struct 
	{
	    int opType;
	    std::string dbName; 
	    std::string tbName; 
	    sqlite_int64 rowId;
	} SQLiteEvent;



/** SQLite update hook interface.

 @ingroup m02
*/
 class SQLiteUpdateHook
{
 public:

 private:

 protected:

    SQLiteUpdateHook () {};
    ~SQLiteUpdateHook () {};

 public:

    /** Method callback on hook registration with the SQLiteQueueThreadExec object.
     */
    virtual void registerCallback (SQLiteQueueThreadExec* emitter) = 0;
    
    virtual void eventCallback (SQLiteQueueThreadExec* emitter,
				const SQLiteEvent& event) = 0;


};



}
}


#endif

