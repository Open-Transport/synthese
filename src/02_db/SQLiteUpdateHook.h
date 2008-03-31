#ifndef SYNTHESE_UTIL_SQLITEUPDATEHOOK_H
#define SYNTHESE_UTIL_SQLITEUPDATEHOOK_H


#include "01_util/threads/ThreadExec.h"

#include <vector>
#include <string>
#include <deque>

#include <boost/filesystem/path.hpp>

#include <sqlite3.h>


namespace synthese
{


namespace db
{

    class SQLiteHandle;



    typedef 
	struct 
	{
	    int opType;
	    std::string dbName; 
	    std::string tbName; 
	    sqlite_int64 rowId;
	} SQLiteEvent;



/** SQLite update hook interface.

 @ingroup m10
*/
 class SQLiteUpdateHook
{
 public:

 private:

 protected:

    SQLiteUpdateHook () {};
    ~SQLiteUpdateHook () {};

 public:

    /** Method callback on hook registration with the SQLiteHandle object.
     */
    virtual void registerCallback (SQLiteHandle* emitter) = 0;
    
    virtual void eventCallback (SQLiteHandle* emitter,
				const SQLiteEvent& event) = 0;


};



}
}


#endif

