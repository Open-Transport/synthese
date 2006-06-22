#ifndef SYNTHESE_UTIL_SQLITEUPDATEHOOK_H
#define SYNTHESE_UTIL_SQLITEUPDATEHOOK_H


#include "module.h"

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

    class SQLiteThreadExec;



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

 protected:

    SQLiteUpdateHook () {};
    ~SQLiteUpdateHook () {};

 public:

    /** Method callback on hook registration with the SQLiteThreadExex object.
     */
    virtual void registerCallback (const SQLiteThreadExec* emitter) = 0;
    
    virtual void eventCallback (const SQLiteThreadExec* emitter,
				const SQLiteEvent& event) = 0;


};



}
}


#endif
