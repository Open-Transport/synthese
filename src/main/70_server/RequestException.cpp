#include "RequestException.h"


namespace synthese
{
namespace server
{


RequestException::RequestException ( const std::string& message)
    : synthese::util::Exception (message)
{
}



RequestException::~RequestException () throw ()
{

}





}
}


