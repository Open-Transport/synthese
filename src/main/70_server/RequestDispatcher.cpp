#include "RequestDispatcher.h"

#include "Request.h"
#include "RequestHandler.h"
#include "RequestException.h"


namespace synthese
{
namespace server
{



RequestDispatcher::RequestDispatcher ()
{
}


RequestDispatcher::~RequestDispatcher ()
{
}





void
RequestDispatcher::dispatchRequest (const Request& request, 
				    std::ostream& stream) const
{
    std::map<std::string, const RequestHandler*>::const_iterator it =
	_handlers.find (request.getFunctionCode ());
    if (it == _handlers.end ()) 
    {
	throw RequestException (std::string ("Unknown request function code : ") + 
				request.getFunctionCode ());
    }
    it->second->handleRequest (request, stream);

}



void 
RequestDispatcher::registerHandler (const RequestHandler* handler)
{
    _handlers.insert (std::make_pair (handler->getFunctionCode (),
				      handler));
}







}
}
