#ifndef SYNTHESE_DB_SQLITEEXCEPTION_H
#define SYNTHESE_DB_SQLITEEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace db
{


/** SQLite related exception class
@ingroup m02
*/

class SQLiteException : public synthese::util::Exception
{
 private:

 public:

    SQLiteException ( const std::string& message);
    ~SQLiteException () throw ();

 private:


};




}

}
#endif

