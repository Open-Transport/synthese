#ifndef SYNTHESE_DB_SQLITE_H
#define SYNTHESE_DB_SQLITE_H

#include "module.h"
#include <string>

#include <boost/filesystem/path.hpp>
#include <sqlite/sqlite3.h>

#include "02_db/SQLiteResult.h"


namespace synthese
{

namespace db
{


/** Wrapper class for SQLite calls (more C++ friendly).
    The return codes must be treated at this level and
    transformed into exceptions.

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
    static sqlite3* OpenConnection (const boost::filesystem::path& databaseFile);
    static void CloseConnection (sqlite3* connection);
    
    static void ExecUpdate (sqlite3* connection, const std::string& sql);
    static SQLiteResult ExecQuery (sqlite3* connection, const std::string& sql);

    //@}



};



}
}


#endif
