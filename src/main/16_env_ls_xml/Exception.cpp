#include "Exception.h"


namespace synthese
{
namespace envlsxml
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



