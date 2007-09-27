#ifndef SYNTHESE_DB_SQLITE_H
#define SYNTHESE_DB_SQLITE_H


#include "02_db/SQLiteStatement.h"
#include "02_db/SQLiteResult.h"
#include <string>
#include <boost/filesystem/path.hpp>


namespace synthese
{

namespace db
{



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
    static bool IsStatementComplete (const SQLData& sql);
    
    static bool IsUpdateStatement (const SQLData& sql);


    static SQLiteStatementSPtr CompileStatement (sqlite3* handle, const SQLData& sql);

    static SQLiteResultSPtr ExecQuery (const SQLiteStatementSPtr& statement, bool lazy = true);
    static SQLiteResultSPtr ExecQuery (sqlite3* handle, const SQLData& sql, bool lazy = true);
    // static SQLiteResultSPtr ExecQuery2 (sqlite3* handle, const SQLData& sql, bool lazy = true);

    static void ExecUpdate (const SQLiteStatementSPtr& statement);
    static void ExecUpdate (sqlite3* handle, const SQLData& sql);


    /** Returns true if a transaction is already opened.
	SQLite does not support nested transaction.
     */
    static bool IsTransactionOpened (sqlite3* handle);

    static void BeginTransaction (sqlite3* handle, bool exclusive = false);
    static void CommitTransaction (sqlite3* handle);
    static void RollbackTransaction (sqlite3* handle);


    //@}



};



}
}


#endif

