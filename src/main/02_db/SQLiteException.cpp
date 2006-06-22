#include "SQLiteException.h"


namespace synthese
{
namespace db
{


SQLiteException::SQLiteException ( const std::string& message)
    : synthese::util::Exception (message)
{
}



SQLiteException::~SQLiteException () throw ()
{

}





}
}


