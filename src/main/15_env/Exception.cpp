#include "Exception.h"


namespace synthese
{
namespace env
{


Exception::Exception ( const std::string& message)
    : synthese::util::Exception (message)
{
}



Exception::~Exception () throw ()
{

}





}
}


