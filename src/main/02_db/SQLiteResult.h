#ifndef SYNTHESE_DB_SQLITERESULT_H
#define SYNTHESE_DB_SQLITERESULT_H


#include "02_db/SQLiteValue.h"

#include <iostream>
#include <vector>



namespace synthese
{

namespace db
{
    class SQLiteResult;

    typedef boost::shared_ptr<SQLiteResult> SQLiteResultSPtr;
    typedef std::vector<SQLiteValue*> SQLiteResultRow;
    

/** Interface for access to a SQLite query result.

 @ingroup m02
*/
class SQLiteResult 
{
 public:

 protected:

    SQLiteResult (); 
    virtual ~SQLiteResult ();
    
 public:

    //! @name Query methods.
    //@{

    virtual void reset () const = 0;
    virtual bool next () const = 0;

    virtual int getNbColumns () const = 0;

    virtual std::string getColumnName (int column) const = 0;
    virtual int getColumnIndex (const std::string& columnName) const;

    virtual SQLiteValue* getValue (int column) const = 0;
    virtual SQLiteValue* getValue (const std::string& name) const;

    virtual std::string getText (int column) const;
    virtual std::string getText (const std::string& name) const;

    virtual int getInt (int column) const;
    virtual int getInt (const std::string& name) const;

    virtual long getLong (int column) const;
    virtual long getLong (const std::string& name) const;

    virtual bool getBool (int column) const;
    virtual bool getBool (const std::string& name) const;

    virtual boost::logic::tribool getTribool (int column) const;
    virtual boost::logic::tribool getTribool (const std::string& name) const;

    virtual double getDouble (int column) const;
    virtual double getDouble (const std::string& name) const;

    virtual std::string getBlob (int column) const;
    virtual std::string getBlob (const std::string& name) const;

    virtual long long getLongLong (int column) const;
    virtual long long getLongLong (const std::string& name) const;

    virtual boost::posix_time::ptime getTimestamp (int column) const;
    virtual boost::posix_time::ptime getTimestamp (const std::string& name) const;

    virtual SQLiteResultRow getRow () const;

    std::vector<int> computeMaxColWidths () const;
    

};



std::ostream& operator<< ( std::ostream& os, const SQLiteResult& op );

}
}



#endif

