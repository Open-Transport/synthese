#include "ThreadException.h"


namespace synthese
{
namespace util
{


ThreadException::ThreadException ( const std::string& message)
    : synthese::util::Exception (message)
{
}



ThreadException::~ThreadException () throw ()
{

}





}
}


