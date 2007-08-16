#ifndef SYNTHESE_DB_SQLITEVALUE_H
#define SYNTHESE_DB_SQLITEVALUE_H


#include "02_db/SQLiteValue.h"
#include <sqlite/sqlite3.h>


#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/logic/tribool.hpp>


namespace synthese
{

namespace db
{

    

/** Interface for access to an SQLite value.

 @ingroup m02
*/
class SQLiteValue
{
 public:

 private:

    std::string _value;  

 public:

    SQLiteValue (sqlite3_value* value); 
    SQLiteValue (const std::string& value); 

    virtual ~SQLiteValue ();

    virtual const std::string& getText () const;
    virtual int getInt () const;
    virtual long getLong () const;
    virtual bool getBool () const;
    virtual boost::logic::tribool getTribool () const;
    virtual double getDouble () const;
    virtual const std::string& getBlob () const;
    virtual long long getLongLong () const;
    virtual boost::posix_time::ptime getTimestamp () const;

  private:


};


typedef boost::shared_ptr<SQLiteValue> SQLiteValueSPtr;

}
}


#endif

