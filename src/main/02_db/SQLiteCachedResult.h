#ifndef SYNTHESE_DB_SQLITECACHEDRESULT_H
#define SYNTHESE_DB_SQLITECACHEDRESULT_H

#include "02_db/SQLiteResult.h"


namespace synthese
{

namespace db
{


/** Cached SQLite result.

 @ingroup m02
*/
class SQLiteCachedResult : public SQLiteResult
{
 public:

 private:

    mutable int _pos;
    std::vector<std::string> _columnNames;
    std::vector<SQLiteResultRow> _rows;
    

 public:

    /** Constructs a new SQLite result by storing in memory all values contained 
     * in another SQLite result.
     */
    SQLiteCachedResult ();
    SQLiteCachedResult (const std::vector<std::string>& columnNames);
    SQLiteCachedResult (const SQLiteResultSPtr& result);
    ~SQLiteCachedResult ();
    
    //! @name Query methods.
    //@{

    void reset () const;
    bool next () const;

    int getNbColumns () const;

    std::string getColumnName (int column) const;

    SQLiteValue* getValue (int column) const;

    SQLiteResultRow getRow () const;
    
    void addRow (const SQLiteResultRow& row);
    void addRow (int nbColumns, char** values, char** columns);

    //@}

 private:

    friend class SQLite;

    

};





}
}


#endif

