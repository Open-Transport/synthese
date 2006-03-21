#include "SocketException.h"


namespace synthese
{
namespace tcp
{


SocketException::SocketException ( const std::string& message)
    : synthese::util::Exception (message)
{
}



SocketException::~SocketException () throw ()
{

}





}
}


