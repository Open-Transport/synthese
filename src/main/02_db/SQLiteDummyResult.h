#ifndef SYNTHESE_DB_SQLITEDUMMYRESULT_H
#define SYNTHESE_DB_SQLITEDUMMYRESULT_H

#include "02_db/SQLiteResult.h"


namespace synthese
{

namespace db
{


/** Dummy SQLite result.

 @ingroup m02
*/
class SQLiteDummyResult : public SQLiteResult
{
 public:

 private:

    SQLiteDummyResult ();

 public:

    ~SQLiteDummyResult ();
    
    //! @name Query methods.
    //@{

    void reset () const;
    bool next () const;

    int getNbColumns () const;

    std::string getColumnName (int column) const;
    int getColumnIndex (const std::string& columnName) const;

    SQLiteValue* getValue (int column) const;
    std::string getText (int column) const;
    int getInt (int column) const;
    long getLong (int column) const;
    bool getBool (int column) const;
    double getDouble (int column) const;
    std::string getBlob (int column) const;
    long long getLongLong (int column) const;
    boost::posix_time::ptime getTimestamp (int column) const;


    //@}

 private:

    friend class SQLite;

    

};





}
}


#endif

