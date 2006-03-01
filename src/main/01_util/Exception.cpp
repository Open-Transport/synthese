#include "Exception.h"


namespace synthese
{
namespace util
{



Exception::Exception ( const std::string& message )
    : _message (message)
{
}


Exception::~Exception ()
{
}



const std::string& 
Exception::getMessage () const
{
    return _message;
}



std::ostream& 
operator<< ( std::ostream& os, const Exception& op )
{
    os << op.getMessage ();
}




}
}
