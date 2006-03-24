#include "Exception.h"


namespace synthese
{
namespace lsxml
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


