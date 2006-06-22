#ifndef SYNTHESE_UTIL_SQLITETHREADEXEC_H
#define SYNTHESE_UTIL_SQLITETHREADEXEC_H


#include "module.h"

#include "01_util/ThreadExec.h"

#include "SQLiteUpdateHook.h"
#include "SQLiteResult.h"


#include <vector>
#include <string>
#include <deque>

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <sqlite/sqlite3.h>


namespace synthese
{


namespace db
{


    void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId);


/** SQLite thread body class.

    This thread is an interface to the SQLite embedded database. It owns
    a dedicated SQLite db handle (connection) which is not released until
    the thread dies. Only one database file is managed per instance of this class.

    The interface wraps common SQLite operations into a C++ interface
    and wraps the result into STL containers.
    The other reason for this interface to exist is proper handling of update hooks
    triggered on db modification. To do so, this class manages internally a queue
    of db events which are treated in FIFO mode inside the body of the thread. 
    
    This guarantees that the following SQLite constraints are satisfied :
    - The db handle is used by one and only one thread, the one which created it
    - No database access is performed inside the body of sql3_update_hook 

 @ingroup m02
*/
 class SQLiteThreadExec : public synthese::util::ThreadExec
{
 public:

 private:

    std::deque<SQLiteEvent> _eventQueue;

    const boost::filesystem::path _databaseFile;
    sqlite3* _db;  //!< SQLite db handle (connection).
    std::vector<SQLiteUpdateHook*> _hooks;   //!< Hooks to trigger on db update.

    boost::shared_ptr<boost::mutex> _hooksMutex; 
    boost::shared_ptr<boost::mutex> _queueMutex; 
    boost::shared_ptr<boost::mutex> _dbMutex; 

 public:
    
    SQLiteThreadExec (const boost::filesystem::path& databaseFile);
    ~SQLiteThreadExec ();

    
    //! @name Query methods.
    //@{
 private:
    bool hasEnqueuedEvent () const;

    //@}


    //! @name Update methods.
    //@{
    void enqueueEvent (const SQLiteEvent& event);
    SQLiteEvent dequeueEvent ();
    void postEvent (const SQLiteEvent& event) const;

 public:

    void registerUpdateHook (SQLiteUpdateHook* hook);

    //@}


    //! @name SQLite db access methods.
    //@{
    SQLiteResult execQuery (const std::string& sql) const;
    void execUpdate (const std::string& sql) const;
    //@}


    void initialize ();
    void loop ();
    void finalize ();

    friend void sqliteUpdateHook (void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId);

};



}
}


#endif
