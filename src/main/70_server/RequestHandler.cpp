#include "RequestHandler.h"


namespace synthese
{
namespace server
{


RequestHandler::RequestHandler (const std::string& functionCode)
    : _functionCode (functionCode)
{
}



RequestHandler::RequestHandler (const RequestHandler& ref)
    : _functionCode (ref._functionCode)
{
}



RequestHandler::~RequestHandler ()
{
}




const std::string& 
RequestHandler::getFunctionCode () const
{
    return _functionCode;
}











}
}
