#ifndef SYNTHESE_DB_SQLITELAZYRESULT_H
#define SYNTHESE_DB_SQLITELAZYRESULT_H

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteStatement.h"


namespace synthese
{

namespace db
{


/** Implementation class for access to a SQLite query result.

 @ingroup m10
*/
class SQLiteLazyResult : public SQLiteResult
{
 public:

 private:

    SQLiteStatementSPtr _statement;
    mutable int _pos;

    SQLiteLazyResult (SQLiteStatementSPtr statement);

 public:

    ~SQLiteLazyResult ();
    
    //! @name Query methods.
    //@{

    void reset () const;
    bool next () const;

    int getNbColumns () const;

    std::string getColumnName (int column) const;

    SQLiteValue* getValue (int column) const;


    //@}

 private:

    friend class SQLiteHandle;

    

};





}
}


#endif

