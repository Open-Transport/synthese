#include "Exception.h"


namespace synthese
{
namespace util
{



Exception::Exception ( const std::string& message ) throw ()
    : _message (message)
{
}


Exception::~Exception () throw ()
{
}



Exception::Exception ( const Exception& ref )
    : _message (ref._message)
{
}



const std::string& 
Exception::getMessage () const
{
    return _message;
}






}
}
