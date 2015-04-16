#include "ThreadException.h"


namespace synthese
{
namespace util
{


ThreadException::ThreadException ( const std::string& message)
    : synthese::Exception (message)
{
}



ThreadException::~ThreadException () throw ()
{

}





}
}



