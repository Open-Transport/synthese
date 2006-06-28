#ifndef SYNTHESE_DB_DBRESULT_H
#define SYNTHESE_DB_DBRESULT_H

#include "module.h"
#include <string>



namespace synthese
{

namespace db
{


/** Interface class for access to a db query result.

 @ingroup m02
*/
class DbResult
{
 protected:

    DbResult () {};
    ~DbResult () {};
    
 public:
    
    //! @name Query methods.
    //@{
    virtual int getNbColumns () const = 0;
    virtual int getNbRows () const = 0;

    virtual std::string getColumnName (int column) const = 0;
    virtual int getColumnIndex (const std::string& columnName) const = 0;

    virtual std::string getColumn (int row, int column) const = 0;
    virtual std::string getColumn (int row, const std::string& name) const = 0;
    //@}

    //! @name Update methods.
    //@{
    //@}


};



}
}


#endif
