#include "InterfacePageException.h"

namespace synthese
{
namespace interfaces
{


InterfacePageException::InterfacePageException ( const std::string& message)
    : synthese::Exception (message)
{
}



InterfacePageException::~InterfacePageException () throw ()
{

}





}
}



