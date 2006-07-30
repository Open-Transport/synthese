#ifndef SYNTHESE_DB_SQLITESYNC_H
#define SYNTHESE_DB_SQLITESYNC_H


#include "02_db/SQLiteUpdateHook.h"

#include <map>
#include <string>
#include <iostream>

#include <boost/thread/recursive_mutex.hpp>


namespace synthese
{
namespace db
{

    class SQLiteThreadExec;
    class SQLiteTableSync;



/** SQLite synchronizer class.
    Manages a set of table synchronizers which synchronize 
    db data with live object model.

@ingroup m02
*/

class SQLiteSync : public synthese::db::SQLiteUpdateHook
{
 private:

    std::string _idColumnName;
    bool _isRegistered;

    std::map<std::string, SQLiteTableSync* > _tableSynchronizers;
    mutable boost::recursive_mutex _tableSynchronizersMutex; 

 public:

    SQLiteSync (const std::string& idColumnName);
    ~SQLiteSync ();

    void registerCallback (const SQLiteThreadExec* emitter);
    
    void eventCallback (const SQLiteThreadExec* emitter,
			const SQLiteEvent& event);

    /** Adds a new table synchronizer to this global synchronizer update hook.
	This method is thread-safe.
    */
    void addTableSynchronizer (SQLiteTableSync* synchronizer);

    bool hasTableSynchronizer (const std::string& tableName) const;
    SQLiteTableSync* getTableSynchronizer (const std::string& tableName) const;
    const std::map<std::string, SQLiteTableSync* >& getTableSynchronizers () const;

 private:


};




}

}
#endif
