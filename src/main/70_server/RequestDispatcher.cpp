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
    // Fetch the site key dans la requete
    
    // on remonte a l'uid du site et on recupere un site*
    
    

    // Verification de l'intervalle de dates

    // si pas bon exception


    std::map<std::string, const RequestHandler*>::const_iterator it =
	_handlers.find (request.getFunctionCode ());
    if (it == _handlers.end ()) 
    {
	throw RequestException (std::string ("Unknown request function code : ") + 
				request.getFunctionCode ());
    }

    // rajouter le param site dans le handle request
    it->second->handleRequest (request, stream);

}










}
}
