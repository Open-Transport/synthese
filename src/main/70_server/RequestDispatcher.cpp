#include "RequestDispatcher.h"

#include "Request.h"
#include "RequestHandler.h"
#include "RequestException.h"


namespace synthese
{
namespace server
{

RequestDispatcher* RequestDispatcher::_instance = 0;
    


RequestDispatcher::RequestDispatcher ()
{
}


RequestDispatcher::~RequestDispatcher ()
{
}


RequestDispatcher* 
RequestDispatcher::getInstance ()
{
    if (_instance == 0)
    {
	_instance = new RequestDispatcher ();
    }
    return _instance;
}




void
RequestDispatcher::dispatchRequest (const Request& request, 
				    std::ostream& stream) const
{
    std::map<std::string, const RequestHandler*>::const_iterator it =
	_handlers.find (request.getFunctionCode ());
    if (it == _handlers.end ()) 
    {
	std::cout << "POUF" << std::flush << std::endl;
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
