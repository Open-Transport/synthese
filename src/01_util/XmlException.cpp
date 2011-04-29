#include "XmlException.h"


namespace synthese
{
namespace util
{


XmlException::XmlException ( const std::string& message)
    : synthese::Exception (message)
{
}



XmlException::~XmlException () throw ()
{

}





}
}



