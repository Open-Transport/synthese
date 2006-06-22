#ifndef SYNTHESE_DB_SQLITERESULT_H
#define SYNTHESE_DB_SQLITERESULT_H

#include "module.h"

#include "DbResult.h"

#include <string>
#include <vector>


namespace synthese
{

namespace db
{


/** Implementation class for access to a SQLite query result.

 @ingroup m02
*/
class SQLiteResult : public DbResult
{
 private:

    typedef std::vector<std::string> Row;

    int _nbColumns;
    int _nbRows;
    std::vector<std::string> _columnNames;
    std::vector<Row> _values;



 public:

    SQLiteResult ();
    ~SQLiteResult ();
    
    //! @name Query methods.
    //@{
    int getNbColumns () const;
    int getNbRows () const;

    const std::string& getColumnName (int column) const;
    int getColumnIndex (const std::string& columnName) const;

    const std::string& getColumn (int row, int column) const;
    const std::string& getColumn (int row, const std::string& name) const;
    //@}

    //! @name Update methods.
    //@{
    void addRow (int nbColumns, char** values, char** columns);
    //@}

};



}
}


#endif
