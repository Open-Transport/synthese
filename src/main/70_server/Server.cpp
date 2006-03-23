#include "Server.h"

#include "ServerThread.h"


#include "RequestException.h"
#include "Request.h"
#include "RequestDispatcher.h"

#include <boost/thread/thread.hpp>

#ifdef MODULE_39
#include "39_carto/MapRequestHandler.h"
#endif


namespace synthese
{
namespace server
{



Server::Server (int port, int nbThreads)
    : _port (port)
    , _nbThreads (nbThreads)
{
}



Server::~Server ()
{
}








    


void 
Server::registerHandlers ()
{
#ifdef MODULE_39
    synthese::server::RequestDispatcher::getInstance ()->
	registerHandler (new synthese::carto::MapRequestHandler ());
#endif
    

}




void 
Server::run () 
{
    registerHandlers ();

    synthese::tcp::TcpService* service = 
	synthese::tcp::TcpService::openService (_port);
    
    // Create the thread group.
    boost::thread_group threads;

    ServerThread serverThread (service);

    // Creates all server threads.
    for (int i=0; i< _nbThreads; ++i) 
    {
	threads.create_thread (serverThread);
    }
      
    threads.join_all();

    synthese::tcp::TcpService::closeService (_port);
}






}
}




