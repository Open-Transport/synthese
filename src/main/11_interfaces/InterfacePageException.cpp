#include "InterfacePageException.h"

namespace synthese
{
namespace interfaces
{


InterfacePageException::InterfacePageException ( const std::string& message)
    : synthese::util::Exception (message)
{
}



InterfacePageException::~InterfacePageException () throw ()
{

}





}
}


