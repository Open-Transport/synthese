#include "Server.h"

#include "ServerThread.h"


#include "RequestException.h"
#include "Request.h"
#include "RequestDispatcher.h"

#include "01_util/Log.h"

#include <boost/thread/thread.hpp>

#ifdef MODULE_39
#include "39_carto/MapRequestHandler.h"
#endif

using synthese::util::Log;


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
    Log::GetInstance ().info ("Starting server...");

    registerHandlers ();

    try 
    {
	synthese::tcp::TcpService* service = 
	    synthese::tcp::TcpService::openService (_port);
	
	ServerThread serverThread (service);
	
	if (_nbThreads == 1) 
	{
	    // Monothread execution ; easier for debugging
	    Log::GetInstance ().info ("Server ready.");
	    serverThread ();
	}
	{
	    // Create the thread group.
	    boost::thread_group threads;
	    
	    // Creates all server threads.
	    for (int i=0; i< _nbThreads; ++i) 
	    {
		threads.create_thread (serverThread);
	    }
	    Log::GetInstance ().info ("Server ready.");
	    threads.join_all();
	}
	
    }
    catch (synthese::util::Exception& ex)
    {
	Log::GetInstance ().fatal ("Error during server init", ex);
    } 


    synthese::tcp::TcpService::closeService (_port);
}






}
}




