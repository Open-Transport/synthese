#ifndef SYNTHESE_DB_SQLITE_H
#define SYNTHESE_DB_SQLITE_H


#include <string>

#include <boost/filesystem/path.hpp>
#include <sqlite/sqlite3.h>

#include "02_db/SQLiteResult.h"
#include "02_db/DBModule.h"

namespace synthese
{

namespace db
{


typedef sqlite3_stmt* SQLiteStatement;


/** Wrapper class for SQLite calls (more C++ friendly).
    The return codes must be treated at this level and
    transformed into exceptions.

    IMPORTANT : to fasten execution of queries, it is crucial
    to wrap them into transations (the exexcution time is about
    60x shorter!!)

 @ingroup m02
*/
class SQLite
{
 private:

    SQLite ();
    ~SQLite ();
    
 public:
    
    
    //! @name Query methods.
    //@{
    static sqlite3* OpenHandle (const boost::filesystem::path& databaseFile);
    static void CloseHandle (sqlite3* handle);

    /** Returns true if the statement is complete (ready to be executed).
	Useful for command line parsing.
    */
    static bool IsStatementComplete (const std::string& sql);
    
    static bool IsUpdateStatement (const std::string& sql);

    static SQLiteStatement PrepareStatement (sqlite3* handle, const std::string& sql);
    static void FinalizeStatement (const SQLiteStatement& statement);

    static void ExecUpdate (sqlite3* handle, const std::string& sql);
    static SQLiteResult ExecQuery (sqlite3* handle, const std::string& sql);

    /** Returns true if a transaction is already opened.
	SQLite does not support nested transaction.
     */
    static bool IsTransactionOpened (sqlite3* handle);

    static void BeginTransaction (sqlite3* handle, bool exclusive = false);
    static void CommitTransaction (sqlite3* handle);


    //@}



};



}
}


#endif

